#ifndef __TYPE_H__
#define __TYPE_H__

#include <string>
#include <map>

class Symbol;

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

        Type(std::string name, ValueType type, int size, bool _const, Type* nonc = NULL);
        virtual ~Type();

        std::string name;
        ValueType vt;
        int size;
        bool _const;
        Type *nonconst_counterpart;
        int start_index;
        Type *array_type;
        std::map<std::string, Symbol*> typeMap; //used for record types

        std::string toString() const;

        static Type* typeFunction() { static Type* t = new Type("function", Function, 0, false); return t; }
        static Type* typeProcedure() { static Type* t = new Type("procedure", Procedure, 0, false); return t; }
        static Type* typeRecord() { static Type* t = new Type("record", Record, 0, false); return t; }

        static std::string constStringId() { return "0string"; }
        static std::string constIntegerId() { return "0integer"; }
        static std::string constCharId() { return "0char"; }
        static std::string constBoolId() { return "0boolean"; }
        static std::string integerId() { return "integer"; }
        static std::string charId() { return "char"; }
        static std::string boolId() { return "boolean"; }
        static std::string uintegerId() { return "INTEGER"; }
        static std::string ucharId() { return "CHAR"; }
        static std::string uboolId() { return "BOOLEAN"; }

        bool isConst();
        static bool isFoldable(ValueType type);
        static bool match(ValueType lhs, ValueType rhs);
        Type* nonconst_val();
        static ValueType const_val(ValueType type);
        static std::string toString(ValueType);
        static std::string prefix(ValueType);
};

#endif //__TYPE_H__
