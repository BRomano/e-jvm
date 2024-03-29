#pragma once

/**
 * Constant pool tags
 */
#define CONSTANT_Class					7
#define CONSTANT_Fieldref				9
#define CONSTANT_Methodref				10
#define CONSTANT_InterfaceMethodref	11
#define CONSTANT_String					8
#define CONSTANT_Integer				3
#define CONSTANT_Float					4
#define CONSTANT_Long					5
#define CONSTANT_Double					6
#define CONSTANT_NameAndType			12
#define CONSTANT_Utf8					1
#define CONSTANT_MethodHandle			15
#define CONSTANT_MethodType			16
#define CONSTANT_InvokeDynamic		18


/** values for variable access_flags in class ClassFile
 * Class access and property modifiers
 */
#define ACC_PUBLIC 		0x0001
#define ACC_FINAL 		0x0010
#define ACC_SUPER			0x0020
#define ACC_INTERFACE 	0x0200
#define ACC_ABSTRACT 	0x0400
#define ACC_SYNTHETIC 	0x1000
#define ACC_ANNOTATION 	0x2000
#define ACC_ENUM			0x4000


/*
Field access and property flags for variable 
access_flags in struct 
field_info {
    u2             access_flags;
    u2             name_index;
    u2             descriptor_index;
    u2             attributes_count;
    attribute_info attributes[attributes_count];
}
*/
#define ACC_PUBLIC 	0x0001
#define ACC_PRIVATE 	0x0002
#define ACC_PROTECTED 	0x0004
#define ACC_STATIC 	0x0008
#define ACC_FINAL 	0x0010
#define ACC_VOLATILE 	0x0040
#define ACC_TRANSIENT 	0x0080
#define ACC_SYNTHETIC 	0x1000
#define ACC_ENUM 	0x4000

/**
 * Method access and property flags
 * access_flags
 method_info {
    u2             access_flags;
    u2             name_index;
    u2             descriptor_index;
    u2             attributes_count;
    attribute_info attributes[attributes_count];
}
 */
//Method access
#define ACC_PUBLIC 	0x0001 	//Declared public; may be accessed from outside its package.
#define ACC_PRIVATE 	0x0002 	//Declared private; accessible only within the defining class.
#define ACC_PROTECTED 	0x0004 	//Declared protected; may be accessed within subclasses.
#define ACC_STATIC 	0x0008 	//Declared static.
#define ACC_FINAL 	0x0010 	//Declared final; must not be overridden (�5.4.5).
#define ACC_SYNCHRONIZED 	0x0020 //Declared synchronized; invocation is wrapped by a monitor use.
#define ACC_BRIDGE 	0x0040 	//A bridge method, generated by the compiler.
#define ACC_VARARGS 	0x0080 	//Declared with variable number of arguments.
#define ACC_NATIVE 	0x0100 	//Declared native; implemented in a language other than Java.
#define ACC_ABSTRACT 	0x0400 	//Declared abstract; no implementation is provided.
#define ACC_STRICT 	0x0800 	//Declared strictfp; floating-point mode is FP-strict.
#define ACC_SYNTHETIC 	0x1000 	//Declared synthetic; not present in the source code. 


/*TODO trocar por um enum.*/
#define T_BOOLEAN  4  
#define T_CHAR  5  
#define T_FLOAT  6  
#define T_DOUBLE  7  
#define T_BYTE  8  
#define T_SHORT  9  
#define T_INT  10  
#define T_LONG  11  
