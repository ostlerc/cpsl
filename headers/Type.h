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
            Bool,
            Unknown
        };

        Type(ValueType type);
        virtual ~Type();
        static bool isConst(ValueType);
        static bool isFoldable(ValueType type);
        static bool match(ValueType lhs, ValueType rhs);
        static ValueType nonconst_val(ValueType type);
        static ValueType const_val(ValueType type);
        static std::string toString(ValueType);
        static std::string prefix(ValueType);
        static ValueType fromString(std::string type_name, bool is_const = false);
        ValueType vt;

};

#endif //__TYPE_H__
