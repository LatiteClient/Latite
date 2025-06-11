HudModule.prototype.setPos = function (x, y) {
    let rect = this.getRect();
    this.setRect(new Rect(x, y, rect.left + rect.getWidth(), rect.top + rect.getHeight()));
};
HudModule.prototype.getPos = function () {
    return new Vector2(this.getRect().left, this.getRect().top);
};

Vector2.prototype.add = function (vec) {
    return new Vector2(this.x + vec.x, this.y + vec.y);
};
Vector2.prototype.sub = function (vec) {
    return new Vector2(this.x - vec.x, this.y - vec.y);
};
Vector2.prototype.mul = function (vec) {
    return new Vector2(this.x * vec.x, this.y * vec.y);
};
Vector2.prototype.div = function (vec) {
    return new Vector2(this.x / vec.x, this.y / vec.y);
};
Vector2.prototype.distanceTo = function (pos) {
    return Math.sqrt((pos.x - this.x ** 2) + (pos.y - this.y) ** 2);
};

Vector3.prototype.add = function (vec) {
    return new Vector3(this.x + vec.x, this.y + vec.y, this.z + vec.z);
};
Vector3.prototype.sub = function (vec) {
    return new Vector3(this.x - vec.x, this.y - vec.y, this.z - vec.z);
};
Vector3.prototype.mul = function (vec) {
    return new Vector3(this.x * vec.x, this.y * vec.y, this.z * vec.z);
};
Vector3.prototype.div = function (vec) {
    return new Vector3(this.x / vec.x, this.y / vec.y, this.z / vec.z);
};
Vector3.prototype.distanceTo = function (pos) {
    return Math.sqrt((pos.x - this.x) ** 2 + (pos.y - this.y) ** 2 + (pos.z - this.z) ** 2);
};

Color.RED = new Color(1, 0, 0, 1);
Color.GREEN = new Color(0, 1, 0, 1);
Color.BLUE = new Color(0, 0, 1, 1);
Color.WHITE = new Color(1, 1, 1, 1);
Color.BLACK = new Color(0, 0, 0, 1);
Color.prototype.asAlpha = function (opacity) {
    return new Color(this.r, this.g, this.b, opacity);
};
Color.RGB = function (r, g, b, a) {
    return new Color(r / 255, g / 255, b / 255, a ? (a / 255) : 1);
};

Rect.prototype.getWidth = function () {
    return this.right - this.left;
};
Rect.prototype.getHeight = function () {
    return this.bottom - this.top;
};
Rect.prototype.getWidth;

TextColor = {};
TextColor.FORMAT_CHAR = "\u00A7";
TextColor.formatText = function (str) {
    return str.replace("&", TextColor.FORMAT_CHAR);
};
TextColor.OBFUSCATE = TextColor.FORMAT_CHAR + 'k';
TextColor.BOLD = TextColor.FORMAT_CHAR + 'l';
TextColor.ITALIC = TextColor.FORMAT_CHAR + 'o';
TextColor.RESET = TextColor.FORMAT_CHAR + 'r';
TextColor.BLACK = TextColor.FORMAT_CHAR + '0';
TextColor.DARK_BLUE = TextColor.FORMAT_CHAR + '1';
TextColor.DARK_GREEN = TextColor.FORMAT_CHAR + '2';
TextColor.DARK_AQUA = TextColor.FORMAT_CHAR + '3';
TextColor.DARK_RED = TextColor.FORMAT_CHAR + '4';
TextColor.DARK_PURPLE = TextColor.FORMAT_CHAR + '5';
TextColor.GOLD = TextColor.FORMAT_CHAR + '6';
TextColor.GRAY = TextColor.FORMAT_CHAR + '7';
TextColor.DARK_GRAY = TextColor.FORMAT_CHAR + '8';
TextColor.BLUE = TextColor.FORMAT_CHAR + '9';
TextColor.GREEN = TextColor.FORMAT_CHAR + 'a';
TextColor.AQUA = TextColor.FORMAT_CHAR + 'b';
TextColor.RED = TextColor.FORMAT_CHAR + 'c';
TextColor.LIGHT_PURPLE = TextColor.FORMAT_CHAR + 'd';
TextColor.YELLOW = TextColor.FORMAT_CHAR + 'e';
TextColor.WHITE = TextColor.FORMAT_CHAR + 'f';
TextColor.MINECOIN_GOLD = TextColor.FORMAT_CHAR + 'g';
TextColor.MATERIAL_QUARTZ = TextColor.FORMAT_CHAR + 'h';
TextColor.MATERIAL_IRON = TextColor.FORMAT_CHAR + 'i';
TextColor.MATERIAL_NETHERITE = TextColor.FORMAT_CHAR + 'j';
TextColor.MATERIAL_REDSTONE = TextColor.FORMAT_CHAR + 'm';
TextColor.MATERIAL_COPPER = TextColor.FORMAT_CHAR + 'n';
TextColor.MATERIAL_GOLD = TextColor.FORMAT_CHAR + 'p';
TextColor.MATERIAL_EMERALD = TextColor.FORMAT_CHAR + 'q';
TextColor.MATERIAL_DIAMOND = TextColor.FORMAT_CHAR + 's';
TextColor.MATERIAL_LAPIS = TextColor.FORMAT_CHAR + 't';
TextColor.MATERIAL_AMETHYST = TextColor.FORMAT_CHAR + 'u';

Uint8Array.prototype.readInt16 = function (idx) {
    let thi = this;
    let lowOrder = thi[idx];
    let highOrder = thi[idx + 1];
    return (highOrder << 8) | lowOrder;
};
Uint8Array.prototype.readInt32 = function (idx) {
    let thi = this;
    let lowOrder = thi.readInt16(idx);
    let highOrder = thi.readInt16(idx + 2);
    return (highOrder << 16) | lowOrder;
};
Uint8Array.prototype.readInt64AsFloat = function (idx) {
    let lowOrder = this.readInt32(idx);
    let highOrder = this.readInt32(idx + 4);
    return lowOrder + highOrder * (2 ** 32);
};
Uint8Array.prototype.readFloat32 = function (idx) {
    let thi = this;
    let arrayBuf = thi.slice(idx, idx + 4);
    return new Float32Array(arrayBuf)[0];
};
Uint8Array.prototype.readFloat64 = function (idx) {
    let thi = this;
    let arrayBuf = thi.slice(idx, idx + 8);
    return new Float64Array(arrayBuf)[0];
};
Uint8Array.prototype.readString = function (idx) {
    let str = "";
    for (let i = 0; i < this.byteLength; ++i) {
        if (this[i] == 0)
            return str;
        str += String.fromCharCode(this[i]);
    }
    return str;
};

util = {
    bufferToString: function (buf) {
        let str = "";
        for (let i = 0; i < buf.byteLength; ++i) {
            str += String.fromCharCode(buf[i]);
        }
        return str;
    },
    stringToBuffer: function (str) {
        let arr = encodeURIComponent(str).replace(/%([0-9A-F]{2})/g, (s, a) => String.fromCharCode('0x' + a)).split('');
        return new Uint8Array(arr.map(c => c.charCodeAt(0)));
    }
};

script = {
    name: "",
    author: "",
    description: "",
    log: clientMessage
};
