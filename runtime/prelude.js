

function Error(message) {
    this.message = message;
    this.name = 'Error';
}

function ReferenceError(message) {
    this.message = message;
    this.name = 'ReferenceError';
}

function TypeError(message) {
    this.message = message;
    this.name = 'TypeError';
}
