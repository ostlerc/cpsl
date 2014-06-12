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
            Procedure,
            Function,
            Record,
            Array,
            Unknown
        };

        Type(ValueType type, int size);
        virtual ~Type();

        ValueType vt;
        int size;

        static Type* typeFunction() { static Type* t = new Type(Function, 0); return t; }
        static Type* typeProcedure() { static Type* t = new Type(Procedure, 0); return t; }

        static std::string constStringId() { return "0string"; }
        static std::string constIntegerId() { return "0integer"; }
        static std::string constCharId() { return "0char"; }
        static std::string constBoolId() { return "0boolean"; }
        static std::string integerId() { return "integer"; }
        static std::string charId() { return "char"; }
        static std::string boolId() { return "boolean"; }

        static bool isConst(ValueType);
        static bool isFoldable(ValueType type);
        static bool match(ValueType lhs, ValueType rhs);
        static ValueType nonconst_val(ValueType type);
        static ValueType const_val(ValueType type);
        static std::string toString(ValueType);
        static std::string prefix(ValueType);
};

#endif //__TYPE_H__
