#include "language.h"

JsValue* LTOperator(JsValue*, JsValue*);
JsValue* GTOperator(JsValue*, JsValue*);
JsValue* LTEOperator(JsValue*, JsValue*);
JsValue* GTEOperator(JsValue*, JsValue*);

JsValue* strictEqualOperator(JsValue*, JsValue*);
// JsValue* equalOperator(JsValue*, JsValue*);
// JsValue* notOperator(JsValue*);

JsValue* multiplyOperator(JsValue*, JsValue*);
JsValue* subtractOperator(JsValue*, JsValue*);
JsValue* addOperator(JsValue*, JsValue*);

// unary
JsValue* notOperator(JsValue*);
JsValue* typeofOperator(JsValue*);


