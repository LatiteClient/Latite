const { readdirSync, statSync, writeFile, readFileSync, writeFileSync } = require("fs");

const folder = ".";
const watermark = `/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Imrglop <xximrgopxx@gmail.com>, 2023
 */`;
require ("fs");

function loopFiles(path = "") {
    var files = readdirSync(path);
    for (let file of files) {
        let pat = path + "/" + file;
        if (statSync(pat).isDirectory()) {
            loopFiles(pat);
        } else {
			if (file.endsWith(".cpp") || file.endsWidth(".h") {
            writeFileSync(pat, watermark + "\n\n"); // minify
			}
        }
    }
}

loopFiles(folder);