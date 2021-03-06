#/* 
#* SPDX-License-Identifier: Apache-2.0
#* Copyright 2019 Western Digital Corporation or its affiliates.
#* 
#* Licensed under the Apache License, Version 2.0 (the "License");
#* you may not use this file except in compliance with the License.
#* You may obtain a copy of the License at
#* 
#* http:*www.apache.org/licenses/LICENSE-2.0
#* 
#* Unless required by applicable law or agreed to in writing, software
#* distributed under the License is distributed on an "AS IS" BASIS,
#* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#* See the License for the specific language governing permissions and
#* limitations under the License.
#*/
import os

class demo(object):
  def __init__(self):
    self.strDemoName   = "trap_baremetal"
    self.rtos_core     = ""
    self.toolchain     = ""
    self.toolchainPath = ""
    self.strGrpFile    = ""
    self.strLinkFilePrefix = ''

    self.public_defs = [
        'D_BARE_METAL',
        'D_TICK_TIME_MS=4',
        'D_ISR_STACK_SIZE=400',
    ]

    self.listSconscripts = [
      'demo_trap_baremetal',
    ]

    self.listDemoSpecificCFlags = [
    ]

    self.listDemoSpecificLinkerFlags = [
    ]
    
    self.listDemoSpecificTargets = [
      'eh1', 'eh2', 'el2','hifive1'
    ]

