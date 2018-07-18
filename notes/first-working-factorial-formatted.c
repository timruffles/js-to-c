
#include "../runtime/environments.h"
#include "../runtime/global.h"
#include "../runtime/language.h"
#include "../runtime/objects.h"
#include "../runtime/operators.h"
#include "../runtime/strings.h"
#include <stdio.h>

char interned_7_cstring[] = "n";
JsValue *interned_7;
char interned_11_cstring[] = "fact";
JsValue *interned_11;
char interned_18_cstring[] = "console";
JsValue *interned_18;

JsValue *fact_1(Env *env) {
  JsValue *return_2;
  /* ternary */
  JsValue *left_5 = (envGet(env, interned_7) /* n */);
  JsValue *right_6 = (jsValueCreateNumber(3));
  JsValue *conditionalPredicate_4 = (LTOperator(left_5, right_6));
  JsValue *conditionalValue_3;
  if (isTruthy(conditionalPredicate_4)) {
    return_2 = (envGet(env, interned_7) /* n */);
  } else {
    JsValue *left_8 = (envGet(env, interned_7) /* n */);
    JsValue *callee_10 = (envGet(env, interned_11) /* fact */);
    JsValue *left_12 = (envGet(env, interned_7) /* n */);
    JsValue *right_13 = (jsValueCreateNumber(1));
    JsValue *call10Arg_0 = (subtractOperator(left_12, right_13));
    JsValue *args_10[] = {call10Arg_0};
    JsValue *right_9 = (functionRunWithArguments(callee_10, env, args_10, 1));
    return_2 = (multiplyOperator(left_8, right_9));
  }
  return return_2;
}

void userProgram(Env *env) {
  JsValue *fact_1_args[] = {stringCreateFromCString("n")};
  JsValue *functionName_14 = stringCreateFromCString("fact");
  envDeclare(env, functionName_14);
  envSet(env, functionName_14, functionCreate(fact_1, fact_1_args, 1));
  JsValue *object_16 = (envGet(env, interned_18) /* console */);
  JsValue *property_17 = stringCreateFromCString("log");
  JsValue *callee_15 = (objectGet(object_16, property_17));

  JsValue *callee_19 = (envGet(env, interned_11) /* fact */);
  JsValue *call19Arg_0 = (jsValueCreateNumber(5));
  JsValue *args_19[] = {call19Arg_0};
  JsValue *call15Arg_0 = (functionRunWithArguments(callee_19, env, args_19, 1));

  JsValue *args_15[] = {call15Arg_0};
  functionRunWithArguments(callee_15, env, args_15, 1);
}

void initialiseInternedStrings() {
  interned_7 = stringCreateFromCString(interned_7_cstring);
  interned_11 = stringCreateFromCString(interned_11_cstring);
  interned_18 = stringCreateFromCString(interned_18_cstring);
}

int main() {
  initialiseInternedStrings();
  JsValue *global = createGlobalObject();
  Env *globalEnv = envFromGlobal(global);
  userProgram(globalEnv);
  return 0;
}
