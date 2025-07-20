#ifndef DEMOWIDGET_H
#define DEMOWIDGET_H

#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include "computepipeline.h"

class DemoWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    explicit DemoWidget(QWidget* parent = nullptr);
protected:
    void initializeGL() override;
    void paintGL() override;
void PrepareBuffers(int dataSize,
               std::shared_ptr<SSBO>& ssboInput,
               std::shared_ptr<SSBO>& ssboOutput,
               std::shared_ptr<SSBO>& ssboBlockSums,
                   std::vector<uint32_t>& inputData);
private:
    void RunDemoPipeline();
     bool RunComputeShader(ComputePipeline& pipeline,
                                 const std::string& shaderName,
                          int dispatchCount);
    void ReadBuffer(std::shared_ptr<SSBO> ssbo, int elementCount, std::vector<uint32_t>& output);
    void VerifyOutput(const std::vector<uint32_t>& output, int printCount);
};


#endif // DEMOWIDGET_H
