#include "language.h"

#define TO_JS_BOOLEAN(X) ((X) ? getTrue() : getFalse())

typedef enum ComparisonResult {
    GT,
    LT,
    EQ,
    // non-sensical comparison - up to specific operator to decide on rval
    UD
} ComparisonResult;

ComparisonResult numberOnlyComparison(JsValue* left, JsValue* right) {
    if(jsValueType(left) != NUMBER_TYPE
       || jsValueType(left) != NUMBER_TYPE) {
        return UD;
    }

    double delta = jsValueNumber(left) - jsValueNumber(right);
    // as with JS - floating point comparisons are problematic
    return delta == 0
        ? EQ
        : delta > 0
            ? GT
            : LT;
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

