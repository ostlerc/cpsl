#ifndef __TYPE_H__
#define __TYPE_H__

#include <string>

class Type
{
    public:
        enum ValueType {
            Const_Integer,
            Const_String,
            Const_Char,
            Const_Bool,
            Integer,
            Char,
            Bool
        };

        Type(ValueType type);
        virtual ~Type();
        static bool isConst(ValueType);
        static std::string toString(ValueType);
        static std::string prefix(ValueType);
        static ValueType fromString(std::string type_name);
        ValueType vt;

};

#endif //__TYPE_H__
