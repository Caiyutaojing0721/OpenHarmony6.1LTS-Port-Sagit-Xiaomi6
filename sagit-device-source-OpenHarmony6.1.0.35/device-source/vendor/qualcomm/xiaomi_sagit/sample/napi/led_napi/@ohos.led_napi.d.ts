/* Copyright 2025 AlgoIdeas <yu19881234@163.com>.
 *
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
 *
 */

/**
 * @since 9
 * @devices phone, tablet
 * @import import led_napi from '@ohos.led_napi';
 * @permission N/A
 */

declare namespace LedNapi {
    const LED_ON = 1;
    const LED_OFF = 0;

    /**
     * 设置LED灯的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     * @param callback 	回调函数
     */
    function setLedStatusWithCallback(pin: number, status: number, callback: (result: number) => void): void;

    /**
     * 设置LED灯的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     */
    function setLedStatusWithPromise(pin: number, status: number): Promise<number>;

    /**
     * 设置LED灯的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     * @param callback 	回调函数
     */
    function setLedStatus(pin: number, status: number, callback: (result: number) => void): void;

    /**
     * 设置LED灯的开关状态
     * @param pin pin口号
     * @param status 状态，1：开启；0：关闭；
     */
    function setLedStatus(pin: number, status: number): Promise<number>;

    /**
     * 获取LED灯的开关状态
     * @param pin pin口号
     * @param callback 	回调函数
     */
    function getLedStatusWithCallback(pin: number, callback: (result: number) => void): void;

    /**
     * 获取LED灯的开关状态
     * @param pin pin口号
     */
    function getLedStatusWithPromise(pin: number): Promise<number>;

    /**
     * 获取LED灯的开关状态
     * @param pin pin口号
     * @param callback 	回调函数
     */
    function getLedStatus(pin: number, callback: (result: number) => void): void;

    /**
     * 获取LED灯的开关状态
     * @param pin pin口号
     */
    function getLedStatus(pin: number): Promise<number>;
}

export default LedNapi;