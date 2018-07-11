struct JsObject {
    properties: ,
    prototype: *JsObject;
};




JsValue objectGet(JsObject* object, JsString* key) {
    // TODO - should this happen in env?
    assert(object != NULL, "Cannot read property '' of undefined");
}
