
        #include <stdio.h>
        #include "../runtime/environments.h"
        
        char interned_1[] = "a";
char interned_5[] = "console";
char interned_6[] = "log";
char interned_9[] = "one";
        
        
        
        void userProgram(Env* env) {
            envDeclare(env, interned_1);
JsValue* object_3 = (envGet(env, interned_5) /* console */);
            JsValue* property_4 = (envGet(env, interned_6) /* log */);
            JsValue* callee_2 = (objectGet(object_3, property_4));
    
            JsValue* object_7 = (envGet(env, interned_1) /* a */);
            JsValue* property_8 = (envGet(env, interned_9) /* one */);
            JsValue* call2Arg_0 = (objectGet(object_7, property_8));
    
            JsValue args_2[] = {call2Arg_0};
            Env* env_2 = envCreateChild(
               env, 
               callee_2->argumentNames,
               &args_2
            );
            callee_2->fn(env_2)
            ;
        }
        
        int main(int argc, char**argv) {
            Env* global = envCreateRoot();
            userProgram(global);
            return 0;
        }
    
