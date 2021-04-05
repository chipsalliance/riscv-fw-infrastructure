'''
 SPDX-License-Identifier: Apache-2.0
 Copyright 2021 Western Digital Corporation or its affiliates.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http:www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
'''


"""SCons.Tool.as

Tool-specific initialization for as, the generic Posix assembler.

There normally shouldn't be any need to import this module directly.
It will usually be imported through the generic SCons.Tool.Tool()
selection method.

"""


__revision__ = "src/engine/SCons/Tool/as.py rel_2.4.1:3453:73fefd3ea0b0 2015/11/09 03:25:05 bdbaddog"

import SCons.Defaults
import SCons.Tool
import SCons.Util
import os

assemblers = ['as']

ASSuffixes = ['.s', '.asm', '.ASM', '.fps']
ASPPSuffixes = ['.spp', '.SPP', '.sx']
if SCons.Util.case_sensitive_suffixes('.s', '.S'):
    ASPPSuffixes.extend(['.S'])
else:
    ASSuffixes.extend(['.S'])

def generate(env):
    """Add Builders and construction variables for as to an Environment."""
    static_obj, shared_obj = SCons.Tool.createObjBuilders(env)

    for suffix in ASSuffixes:
        static_obj.add_action(suffix, SCons.Defaults.ASAction)
        shared_obj.add_action(suffix, SCons.Defaults.ASAction)
        static_obj.add_emitter(suffix, SCons.Defaults.StaticObjectEmitter)
        shared_obj.add_emitter(suffix, SCons.Defaults.SharedObjectEmitter)

    for suffix in ASPPSuffixes:
        static_obj.add_action(suffix, SCons.Defaults.ASPPAction)
        shared_obj.add_action(suffix, SCons.Defaults.ASPPAction)
        static_obj.add_emitter(suffix, SCons.Defaults.StaticObjectEmitter)
        shared_obj.add_emitter(suffix, SCons.Defaults.SharedObjectEmitter)

    # Tool assembler compiler execution
    env['AS_BIN'] = "clang"

    env['AS']        = os.path.join("$RISCV_LLVM_TC_PATH","bin" ,"$AS_BIN")
    env['ASFLAGS']   = SCons.Util.CLVar('')
    env['ASCOM']     = '$AS $ASFLAGS -o $TARGET $_CCCOMCOM $SOURCES'
    env['ASPPFLAGS'] = '$ASFLAGS'
    env['ASPPCOM']   = '$CC $ASPPFLAGS $CPPFLAGS $_CPPDEFFLAGS $_CPPINCFLAGS -c -o $TARGET $SOURCES'
    env['TOOLCHAIN'] = "RISCV"

def exists(env):
    return env.Detect(assemblers)

# Local Variables:
# tab-width:4
# indent-tabs-mode:nil
# End:
# vim: set expandtab tabstop=4 shiftwidth=4:
