/*
 * Copyright (c) 2022 Unionman Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "display_common.h"
#include "display_gralloc.h"
#include "securec.h"
#include "display_gfx_private.h"
#include "display_gfx.h"

#ifdef QCOM_GFX_ON
static int32_t InitGfx()
{
    DISPLAY_LOGI("Init Gfx\n");
    return DISPLAY_SUCCESS;
}

static int32_t DeinitGfx()
{
    DISPLAY_LOGI("Deinit Gfx\n");
    return DISPLAY_SUCCESS;
}
static int32_t FillRect(ISurface *surface, IRect *rect, uint32_t color, GfxOpt *opt)
{
    DISPLAY_LOGI("FillRect\n");
    return DISPLAY_FAILURE;
}

static int32_t Blit(ISurface *srcSurface, IRect *srcRect, ISurface *dstSurface, IRect *dstRect, GfxOpt *opt)
{
    DISPLAY_LOGI("Blit\n");
    return DISPLAY_FAILURE;
}

static int32_t Sync(int32_t timeOut)
{
    return DISPLAY_SUCCESS;
}
#endif

int32_t GfxInitialize(GfxFuncs **funcs)
{
    DISPLAY_CHK_RETURN((funcs == NULL), DISPLAY_PARAM_ERR, DISPLAY_LOGE("info is null"));
    GfxFuncs *gfxFuncs = (GfxFuncs *)malloc(sizeof(GfxFuncs));
    if (!gfxFuncs) {
        DISPLAY_LOGE("malloc GfxFuncs failed");
        return DISPLAY_FAILURE;
    }

    errno_t eok = memset_s((void *)gfxFuncs, sizeof(GfxFuncs), 0, sizeof(GfxFuncs));
    if (eok != EOK) {
        DISPLAY_LOGE("memset_s failed");
        free(gfxFuncs);
        return DISPLAY_FAILURE;
    }

#ifdef QCOM_GFX_ON
    gfxFuncs->InitGfx = InitGfx;
    gfxFuncs->DeinitGfx = DeinitGfx;
    gfxFuncs->FillRect = FillRect;
    gfxFuncs->Blit = Blit;
    gfxFuncs->Sync = Sync;
    *funcs = gfxFuncs;
#else
    *funcs = NULL;
#endif

    return DISPLAY_SUCCESS;
}

int32_t GfxUninitialize(GfxFuncs *funcs)
{
    if (funcs == NULL) {
        DISPLAY_LOGE("funcs is null and return\n");
        return DISPLAY_NULL_PTR;
    }
    free(funcs);
    DISPLAY_LOGI("%s: gfx uninitialize success", __func__);
    return DISPLAY_SUCCESS;
}
