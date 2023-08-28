const { readdirSync, statSync, writeFile, readFileSync, writeFileSync } = require("fs");

const folder = ".";
const watermark = "#include \"pch.h\"\r\n";
require ("fs");

function loopFiles(path = "") {
    var files = readdirSync(path);
    for (let file of files) {
        let pat = path + "/" + file;
        if (statSync(pat).isDirectory()) {
            loopFiles(pat);
        } else {
            var fil = readFileSync(pat).toString();
			if (pat.endsWith(".cpp")) {
				writeFileSync(pat, watermark + fil.split("#include \"pch.h\"\r\n").join(""), "ascii");
			}
        }
    }
}

loopFiles(folder);