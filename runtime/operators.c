#include "language.h"
#include "strings.h"
#include "operators.h"
#include "lib/debug.h"

#define BOTH_NUMBERS(L,R) !(isNaN(L) || isNaN(R) || jsValueType(L) != NUMBER_TYPE || jsValueType(R) != NUMBER_TYPE)

typedef enum ComparisonResult {
    GT,
    LT,
    EQ,
    // non-sensical comparison - up to specific operator to decide on rval
    UD

} ComparisonResult;

static ComparisonResult deltaToComaparisonResult(double delta) {
    // as with JS - floating point comparisons are problematic
    return delta == 0
        ? EQ
        : delta > 0
            ? GT
            : LT;
}

static ComparisonResult numberOnlyComparison(JsValue* left, JsValue* right) {
    if(jsValueType(left) != NUMBER_TYPE
       || jsValueType(left) != NUMBER_TYPE) {
        return UD;
    }

    return deltaToComaparisonResult(jsValueNumber(left) - jsValueNumber(right));
}

JsValue* LTOperator(JsValue* left, JsValue* right) {
    ComparisonResult r = numberOnlyComparison(left, right); 
    return TO_JS_BOOLEAN(r == LT);
}

JsValue* GTOperator(JsValue* left, JsValue* right) {
    ComparisonResult r = numberOnlyComparison(left, right); 
    return TO_JS_BOOLEAN(r == GT);
}

JsValue* LTEOperator(JsValue* left, JsValue* right) {
    ComparisonResult r = numberOnlyComparison(left, right); 
    return TO_JS_BOOLEAN(r != UD && r != GT);
}

JsValue* GTEOperator(JsValue* left, JsValue* right) {
    ComparisonResult r = numberOnlyComparison(left, right); 
    return TO_JS_BOOLEAN(r != UD && r != LT);
}

JsValue* notOperator(JsValue* left) {
    return isTruthy(left) ? getFalse() : getTrue();
}

JsValue* strictEqualOperator(JsValue* left, JsValue* right) {
    if(left == right) {
        return getTrue();
    }

    JsValueType leftType = jsValueType(left);
    JsValueType rightType = jsValueType(right);
    if(leftType != rightType) {
        return getFalse();
    }

    // for undefined + null, if they're the same type they're the same value
    if(leftType == NUMBER_TYPE) {
        return TO_JS_BOOLEAN(deltaToComaparisonResult(jsValueNumber(left) - jsValueNumber(right)) == EQ);
    } else if(leftType == STRING_TYPE) {
        return TO_JS_BOOLEAN(stringIsEqual(left, right));
    } else {
        // remaining types are
        // 1. objects: same pointer check already failed
        // 2. booleans, null, undefined: analytically, since we have one of these objects, if pointers aren't 
        //    equal the booleans/nulls/undefineds aren't equal
        // 3. functions, also pointer equality
        return getFalse();
    }
}


JsValue* multiplyOperator(JsValue* left, JsValue* right) {
    if(!BOTH_NUMBERS(left, right)) {
        return getNaN();
    }
    return jsValueCreateNumber(jsValueNumber(left) * jsValueNumber(right));
}

JsValue* subtractOperator(JsValue* left, JsValue* right) {
    if(!BOTH_NUMBERS(left, right)) {
        return getNaN();
    }
    return jsValueCreateNumber(jsValueNumber(left) - jsValueNumber(right));
}

JsValue* addOperator(JsValue* left, JsValue* right) {
    if(!BOTH_NUMBERS(left, right)) {
        return getNaN();
    }
    return jsValueCreateNumber(jsValueNumber(left) + jsValueNumber(right));
}

JsValue* typeofOperator(JsValue* val) {
    switch(jsValueType(val)) {
        case  UNDEFINED_TYPE: return stringFromLiteral("undefined");
        case  NUMBER_TYPE: return stringFromLiteral("number");
        case  NAN_TYPE: return stringFromLiteral("number");
        case  BOOLEAN_TYPE: return stringFromLiteral("boolean");
        case  OBJECT_TYPE: return stringFromLiteral("object");
        case  NULL_TYPE: return stringFromLiteral("object");
        case  STRING_TYPE: return stringFromLiteral("string");
        case  FUNCTION_TYPE: return stringFromLiteral("function");
        default:
            fail("Non JSValue %s", jsValueReflect(val).name);
    }
}
