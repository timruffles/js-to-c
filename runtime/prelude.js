function Error(message) {
    // TODO if(!(this instanceof Error)) return new Error(message);
    this.message = message;
}

Error.prototype = {
    name: "Error",
};
