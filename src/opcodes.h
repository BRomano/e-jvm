#pragma once

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "JavaTypes.h"
#include "JavaVM.h"

/**
  This file has all bytecode instructions.
*/

//extern struct opCode _OpcodeArray[];

bool f_fconst_0(u1 * bc, Frame * frame, JavaVM * arg);
bool f_ASSERT(u1 * bc, Frame * frame, JavaVM * arg);
bool f_nop(u1 * bc, Frame * frame, JavaVM * arg);
bool f_aconst_null(u1 * bc, Frame * frame, JavaVM * arg);
bool f_iconst_m1(u1 * bc, Frame * frame, JavaVM * arg);
bool f_bipush(u1 * bc, Frame * frame, JavaVM * arg);
bool f_ldc(u1 * bc, Frame * frame, JavaVM * arg);
bool f_ldc2_w(u1 * bc, Frame * frame, JavaVM * arg);
bool f_iload(u1 * bc, Frame * frame, JavaVM * arg);
bool f_lload(u1 * bc, Frame * frame, JavaVM * arg);
bool f_aload(u1 * bc, Frame * frame, JavaVM * arg);
bool f_iload_0(u1 * bc, Frame * frame, JavaVM * arg);
bool f_lload_0(u1 * bc, Frame * frame, JavaVM * arg);
bool f_lload_1(u1 * bc, Frame * frame, JavaVM * arg);
bool f_lload_2(u1 * bc, Frame * frame, JavaVM * arg);
bool f_lload_3(u1 * bc, Frame * frame, JavaVM * arg);
bool f_fload_0(u1 * bc, Frame * frame, JavaVM * arg);
bool f_aload_0(u1 * bc, Frame * frame, JavaVM * arg);
bool f_iaload(u1 * bc, Frame * frame, JavaVM * arg);
bool f_aaload(u1 * bc, Frame * frame, JavaVM * arg);
bool f_istore(u1 * bc, Frame * frame, JavaVM * arg);
bool f_astore(u1 * bc, Frame * frame, JavaVM * arg);
bool f_istore_0(u1 * bc, Frame * frame, JavaVM * arg);
bool f_lstore_0(u1 * bc, Frame * frame, JavaVM * arg);
bool f_lstore_1(u1 * bc, Frame * frame, JavaVM * arg);
bool f_lstore_2(u1 * bc, Frame * frame, JavaVM * arg);
bool f_lstore_3(u1 * bc, Frame * frame, JavaVM * arg);
bool f_fstore_0(u1 * bc, Frame * frame, JavaVM * arg);
bool f_fstore_1(u1 * bc, Frame * frame, JavaVM * arg);
bool f_fstore_2(u1 * bc, Frame * frame, JavaVM * arg);
bool f_fstore_3(u1 * bc, Frame * frame, JavaVM * arg);
bool f_astore_0(u1 * bc, Frame * frame, JavaVM * arg);
bool f_astore_0(u1 * bc, Frame * frame, JavaVM * arg);
bool f_astore_0(u1 * bc, Frame * frame, JavaVM * arg);
bool f_astore_0(u1 * bc, Frame * frame, JavaVM * arg);
bool f_iastore(u1 * bc, Frame * frame, JavaVM * arg);
bool f_aastore(u1 * bc, Frame * frame, JavaVM * arg);
bool f_pop(u1 * bc, Frame * frame, JavaVM * arg);
bool f_dup(u1 * bc, Frame * frame, JavaVM * arg);
bool f_dup_x1(u1 * bc, Frame * frame, JavaVM * arg);
bool f_dup_x2(u1 * bc, Frame * frame, JavaVM * arg);
bool f_iadd(u1 * bc, Frame * frame, JavaVM * arg);
bool f_fadd(u1 * bc, Frame * frame, JavaVM * arg);
bool f_ladd(u1 * bc, Frame * frame, JavaVM * arg);
bool f_isub(u1 * bc, Frame * frame, JavaVM * arg);
bool f_imul(u1 * bc, Frame * frame, JavaVM * arg);
bool f_iinc(u1 * bc, Frame * frame, JavaVM * arg);
bool f_int2float(u1 * bc, Frame * frame, JavaVM * arg);
bool f_float2int(u1 * bc, Frame * frame, JavaVM * arg);
bool f_fcmp(u1 * bc, Frame * frame, JavaVM * arg);
bool f_ifeq(u1 * bc, Frame * frame, JavaVM * arg);
bool f_if_icmpeq(u1 * bc, Frame * frame, JavaVM * arg);
bool f_goto(u1 * bc, Frame * frame, JavaVM * arg);
bool f_return(u1 * bc, Frame * frame, JavaVM * arg);
bool f_getfield(u1 * bc, Frame * frame, JavaVM * arg);
bool f_putfield(u1 * bc, Frame * frame, JavaVM * arg);
bool f_invokevirtual(u1 * bc, Frame * frame, JavaVM * arg);
bool f_invokespecial(u1 * bc, Frame * frame, JavaVM * arg);
bool f_invokestatic(u1 * bc, Frame * frame, JavaVM * arg);
bool f_new(u1 * bc, Frame * frame, JavaVM * arg);
bool f_newarray(u1 * bc, Frame * frame, JavaVM * arg);
bool f_anewarray(u1 * bc, Frame * frame, JavaVM * arg);
bool f_arraylength(u1 * bc, Frame * frame, JavaVM * arg);
bool f_athrow(u1 * bc, Frame * frame, JavaVM * arg);
bool f_checkcast(u1 * bc, Frame * frame, JavaVM * arg);
bool f_instanceof(u1 * bc, Frame * frame, JavaVM * arg);
bool f_monitorenter(u1 * bc, Frame * frame, JavaVM * arg);
bool f_monitorexit(u1 * bc, Frame * frame, JavaVM * arg);
