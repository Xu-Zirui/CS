#include "DemoWidget.h"
#include <QDebug>
#include "ComputePipeline.h"
#include "ComputeShader.h"
#include "SSBO.h"
#include <random>
#include <QElapsedTimer>
#include <QString>
#include <QDebug>
#include "csresourcemanager.h"
// Run a function and measure its execution time in milliseconds
template<typename Func>
qint64 MeasureExecutionTime(Func&& func, const QString& info = QString())
{
    QElapsedTimer timer;
    timer.start();

    func();

    qint64 elapsed = timer.elapsed();
    if (!info.isEmpty()) {
        qDebug() << info << "time elapsed:" << elapsed << "ms";
    }
    return elapsed;
}

// Simple CPU prefix sum calculation
void PrefixSumCPU(const std::vector<uint32_t>& input, std::vector<uint32_t>& output) {
    output.resize(input.size());
    if (input.empty()) return;
    output[0] = input[0];
    for (size_t i = 1; i < input.size(); ++i) {
        output[i] = output[i-1] + input[i];
    }
}
std::vector<uint32_t> GenerateRandomData(int size, uint32_t minVal = 1, uint32_t maxVal = 100) {
    std::vector<uint32_t> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(minVal, maxVal);

    for (int i = 0; i < size; ++i) {
        data[i] = dist(gen);
    }
    return data;
}
// Print the first N elements of a data vector with a title
void PrintFirstN(const std::vector<uint32_t>& data, int n, const QString& title) {
    QString s;
    for (int i = 0; i < n && i < static_cast<int>(data.size()); ++i) {
        s += QString::number(data[i]) + " ";
    }
    qDebug() << title << s;
}

DemoWidget::DemoWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
}

void DemoWidget::initializeGL()
{
    initializeOpenGLFunctions();
    RunDemoPipeline();
}

void DemoWidget::paintGL()
{
    // Rendering or other operations can be done here
}

void DemoWidget::PrepareBuffers(int dataSize,
                                std::shared_ptr<SSBO>& ssboInput,
                                std::shared_ptr<SSBO>& ssboOutput,
                                std::shared_ptr<SSBO>& ssboBlockSums,
                                std::vector<uint32_t>& inputData)
{
    // Initialize input data with values 1, 2, 3, ...

    inputData = GenerateRandomData(dataSize, 1, dataSize);

    ssboInput = std::make_shared<SSBO>("InputBuffer");
    ssboInput->Create(sizeof(uint32_t) * dataSize, inputData.data());

    ssboOutput = std::make_shared<SSBO>("OutputBuffer");
    ssboOutput->Create(sizeof(uint32_t) * dataSize, nullptr);

    int numBlocks = (dataSize + 256 - 1) / 256;
    ssboBlockSums = std::make_shared<SSBO>("BlockSums");
    ssboBlockSums->Create(sizeof(uint32_t) * numBlocks, nullptr);
}

bool DemoWidget::RunComputeShader(ComputePipeline& pipeline,
                                  const std::string& shaderName,
                                  int dispatchCount)
{
    if (!pipeline.Build(shaderName)) {
        qWarning() << QString::fromStdString(shaderName) << "build failed";
        return false;
    }
    pipeline.Dispatch(shaderName,dispatchCount);
    glFinish(); // Wait for GPU to complete
    return true;
}

void DemoWidget::ReadBuffer(std::shared_ptr<SSBO> ssbo, int elementCount, std::vector<uint32_t>& output)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->Id());
    void* ptr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32_t) * elementCount, GL_MAP_READ_BIT);
    output.resize(elementCount);
    if (ptr) {
        memcpy(output.data(), ptr, sizeof(uint32_t) * elementCount);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void DemoWidget::VerifyOutput(const std::vector<uint32_t>& output, int printCount = 16)
{
    QString s;
    for (int i = 0; i < printCount && i < (int)output.size(); ++i)
        s += QString::number(output[i]) + " ";
    qDebug() << "Output first" << printCount << "values:" << s;
}

void DemoWidget::RunDemoPipeline()
{
    const int dataSize = 10000000; // Test with 100,000 elements
    ResourceManager rm;

    // 加载shader
    rm.LoadShaders({
                    {"BlockScan", "C:/Users/xzr/Documents/Demo/shaders/blockScan.comp"},
                    {"AddBlockSums", "C:/Users/xzr/Documents/Demo/shaders/addBlockSums.comp"},
                    });

    // 创建并初始化SSBO
    std::vector<uint32_t> inputData = GenerateRandomData(dataSize, 1, dataSize);
    rm.CreateSSBOWithData("InputBuffer", inputData);

    // 创建空SSBO和UBO
    rm.CreateSSBOs({
        {"OutputBuffer", dataSize * sizeof(uint32_t)},
        {"BlockSums", ((dataSize + 255) / 256) * sizeof(uint32_t)}
    });


    // 加载所有资源到ComputePipeline
    ComputePipeline pipeline;
    for (auto& [name, shader] : rm.GetAllShaders())
        pipeline.AddShader(name, shader);
    for (auto& [name, ssbo] : rm.GetAllSSBOs())
        pipeline.AddSSBO(name, ssbo);
    for (auto& [name, ubo] : rm.GetAllUBOs())
        pipeline.AddUBO(name, ubo); // 你需要在ComputePipeline中加这个接口

    int numBlocks = (dataSize + 256 - 1) / 256;

    // Run the first shader and measure execution time
    qint64 tBlockScan = MeasureExecutionTime([&]() {
        RunComputeShader(pipeline, "BlockScan", numBlocks);
    }, "BlockScan shader execution");

    // Run the second shader and measure execution time
    qint64 tAddBlockSums = MeasureExecutionTime([&]() {
        RunComputeShader(pipeline, "AddBlockSums", numBlocks);
    }, "AddBlockSums shader execution");

    // Read output buffer and measure time (can be skipped for pure GPU timing)
    std::vector<uint32_t> outputData;


    qint64 tReadOutput = MeasureExecutionTime([&]() {
        ReadBuffer(pipeline.GetSsbos()["OutputBuffer"], dataSize, outputData);
    }, "Reading output buffer");

    VerifyOutput(outputData, 16);

    qDebug() << "=== Timing summary (ms) ===";
    qDebug() << "BlockScan:" << tBlockScan;
    qDebug() << "AddBlockSums:" << tAddBlockSums;
    qDebug() << "ReadOutput:" << tReadOutput;
}
