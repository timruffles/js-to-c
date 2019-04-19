
        #include <stdio.h>
        #include "../runtime/environments.h"
        #include "../runtime/strings.h"
        #include "../runtime/objects.h"
        #include "../runtime/language.h"
        #include "../runtime/operators.h"
        
        char interned_6_cstring[] = "n";
         JsValue* interned_6;
char interned_10_cstring[] = "fact";
         JsValue* interned_10;
char interned_16_cstring[] = "console";
         JsValue* interned_16;
char interned_17_cstring[] = "log";
         JsValue* interned_17;
        
        JsValue* fact_1(Env* env) {
        JsValue* left_4 = (envGet(env, interned_6) /* n */);
            JsValue* right_5 = (jsValueCreateNumber(3));
            JsValue* conditionalResult_3 = (GTOperator(left_4, right_5));;
            if(isTruthy(conditionalResult_3)) {
                JsValue* return_2 = (envGet(env, interned_6) /* n */);
            } else {
                JsValue* left_7 = (envGet(env, interned_6) /* n */);
            JsValue* callee_9 = (envGet(env, interned_10) /* fact */);
            JsValue* left_11 = (envGet(env, interned_6) /* n */);
            JsValue* right_12 = (jsValueCreateNumber(1));
            JsValue* call9Arg_0 = (subtractOperator(left_11, right_12));
            JsValue args_9[] = {call9Arg_0};
            Env* env_9 = envCreateChild(
               env, 
               callee_9->argumentNames,
               &args_9
            );
            JsValue* right_8 = (callee_9->fn(env_9));
            
            JsValue* return_2 = (multiplyOperator(left_7, right_8));
            }
            return return_2
    }
        
        void userProgram(Env* env) {
            const char* fact_1_args[] = {"n"};
            envDeclare(env, stringCreateFromCString("fact"));
            envSet(env, functionCreate(fact_1, 1, fact_1_args));
JsValue* object_14 = (envGet(env, interned_16) /* console */);
            JsValue* property_15 = (envGet(env, interned_17) /* log */);
            JsValue* callee_13 = (objectGet(object_14, property_15));
    
            JsValue* callee_18 = (envGet(env, interned_10) /* fact */);
            JsValue* call18Arg_0 = (jsValueCreateNumber(5));
            JsValue args_18[] = {call18Arg_0};
            Env* env_18 = envCreateChild(
               env, 
               callee_18->argumentNames,
               &args_18
            );
            JsValue* call13Arg_0 = (callee_18->fn(env_18));
            
            JsValue args_13[] = {call13Arg_0};
            Env* env_13 = envCreateChild(
               env, 
               callee_13->argumentNames,
               &args_13
            );
            callee_13->fn(env_13)
            ;
        }
        
        void initialiseInternedStrings() {
        interned_6 = createStringFromCString(interned_6_cstring);
interned_10 = createStringFromCString(interned_10_cstring);
interned_16 = createStringFromCString(interned_16_cstring);
interned_17 = createStringFromCString(interned_17_cstring);
    }
        
        int main(int argc, char**argv) {
            Env* global = envCreateRoot();
            userProgram(global);
            return 0;
        }
    
