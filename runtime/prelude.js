function Error(message) {
    // TODO if(!(this instanceof Error)) return new Error(message);
    this.message = message;
}

Error.prototype = {
    name: "Error",
};

function arrayPush(x) {
    var l = this.length
    this[l] = x
    return l
}

Array.prototype = {
    length: 0,
    push: arrayPush,
};
