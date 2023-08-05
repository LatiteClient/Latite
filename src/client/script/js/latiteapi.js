Vector2.prototype.add = function (vec) {
    return new Vector2(this.x + vec.x, this.y + vec.y);
}

Vector2.prototype.sub = function (vec) {
    return new Vector2(this.x - vec.x, this.y - vec.y);
}

Vector2.prototype.mul = function (vec) {
    return new Vector2(this.x * vec.x, this.y * vec.y);
}

Vector2.prototype.div = function (vec) {
    return new Vector2(this.x / vec.x, this.y / vec.y);
}
