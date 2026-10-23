/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include <sys/mman.h>

#include "securec.h"
#include "camera.h"
#include "aml_codec_node.h"

namespace OHOS::Camera {
#define ENCODER_FRAMERATE (30)
#define ENCODER_BITRATE (2000000)
#define ENCODER_GOP (20)

AMLCodecNode::AMLCodecNode(const std::string& name, const std::string& type) : NodeBase(name, type)
{
    CAMERA_LOGV("%{public}s enter, type(%{public}s)\n", name_.c_str(), type_.c_str());
}

AMLCodecNode::~AMLCodecNode()
{
    CAMERA_LOGI("~AMLCodecNode Node exit.");
}

RetCode AMLCodecNode::Start(const int32_t streamId)
{
    CAMERA_LOGI("AMLCodecNode::Start streamId = %{public}d\n", streamId);
    vencFrameCnt_ = 0;
    return RC_OK;
}

RetCode AMLCodecNode::Stop(const int32_t streamId)
{
    CAMERA_LOGI("AMLCodecNode::Stop streamId = %{public}d\n", streamId);
    return RC_OK;
}

RetCode AMLCodecNode::Flush(const int32_t streamId)
{
    CAMERA_LOGI("AMLCodecNode::Flush streamId = %{public}d\n", streamId);
    return RC_OK;
}

void AMLCodecNode::encodeJpegToMemory(unsigned char* image, int width, int height, \
    unsigned long* jpegSize, unsigned char** jpegBuf)
{
    struct jpeg_compress_struct cInfo;
    struct jpeg_error_mgr jErr;
    JSAMPROW row_pointer[1];
    int row_stride = 0;
    constexpr uint32_t colorMap = 3;
    constexpr uint32_t compressionRatio = 100;
    constexpr uint32_t pixelsThick = 3;

    cInfo.err = jpeg_std_error(&jErr);

    jpeg_create_compress(&cInfo);
    cInfo.image_width = width;
    cInfo.image_height = height;
    cInfo.input_components = colorMap;
    cInfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cInfo);
    jpeg_set_quality(&cInfo, compressionRatio, TRUE);
    jpeg_mem_dest(&cInfo, jpegBuf, jpegSize);
    jpeg_start_compress(&cInfo, TRUE);

    row_stride = width;
    while (cInfo.next_scanline < cInfo.image_height) {
        row_pointer[0] = &image[cInfo.next_scanline * row_stride * pixelsThick];
        jpeg_write_scanlines(&cInfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cInfo);
    jpeg_destroy_compress(&cInfo);
}

void AMLCodecNode::EncodeForPreview(std::shared_ptr<IBuffer>& buffer)
{
    if (buffer == nullptr) {
        CAMERA_LOGE("Error: buffer == nullptr");
        return;
    }

    CAMERA_LOGE("EncodeForPreview");
}

void AMLCodecNode::EncodeForJpeg(std::shared_ptr<IBuffer>& buffer)
{
    CAMERA_LOGE("AMLCodecNode::EncodeForJpeg\n");
}

void AMLCodecNode::EncodeForVideo(std::shared_ptr<IBuffer>& buffer)
{
    CAMERA_LOGE("AMLCodecNode::EncodeForVideo\n");
}

void AMLCodecNode::DeliverBuffer(std::shared_ptr<IBuffer> &buffer)
{
    if (buffer == nullptr) {
        CAMERA_LOGE("AMLCodecNode::DeliverBuffer frameSpec is null");
        return;
    }

    int32_t id = buffer->GetStreamId();
    CAMERA_LOGD("AMLCodecNode::DeliverBuffer ENTER StreamId %{public}d, type: %{public}d",
                id, buffer->GetEncodeType());
    if (buffer->GetBufferStatus() == CAMERA_BUFFER_STATUS_OK) {
        if (buffer->GetEncodeType() == ENCODE_TYPE_JPEG) {
            EncodeForJpeg(buffer);
        } else if (buffer->GetEncodeType() == ENCODE_TYPE_H264 ||
                   buffer->GetEncodeType() == ENCODE_TYPE_H265) {
            EncodeForVideo(buffer);
        } else {
            // previewWidth_ = buffer->GetWidth();
            // previewHeight_ = buffer->GetHeight();

            EncodeForPreview(buffer);
        }
    }

    outPutPorts_ = GetOutPorts();
    for (auto &it : outPutPorts_) {
        if (it->format_.streamId_ == id) {
            it->DeliverBuffer(buffer);
            CAMERA_LOGI("AMLCodecNode deliver buffer streamid = %{public}d", it->format_.streamId_);
            return;
        }
    }
}

RetCode AMLCodecNode::Capture(const int32_t streamId, const int32_t captureId)
{
    CAMERA_LOGV("AMLCodecNode::Capture streamid = %{public}d captureId = %{public}d", streamId, captureId);
    return RC_OK;
}

RetCode AMLCodecNode::CancelCapture(const int32_t streamId)
{
    CAMERA_LOGI("AMLCodecNode::CancelCapture streamid = %{public}d", streamId);

    return RC_OK;
}

REGISTERNODE(AMLCodecNode, {"AMLCodec"})
} // namespace OHOS::Camera
