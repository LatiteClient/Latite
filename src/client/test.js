const { readdirSync, statSync, writeFile, readFileSync, writeFileSync } = require("fs");

const folder = ".";
const watermark = `/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */`;
require ("fs");

function loopFiles(path = "") {
    var files = readdirSync(path);
    for (let file of files) {
        let pat = path + "/" + file;
        if (statSync(pat).isDirectory()) {
            loopFiles(pat);
        } else {
			if (pat.endsWith(".cpp") || pat.endsWith(".h")) {
				writeFileSync(pat, watermark.split("\n").join("\r\n") + "\r\n\r\n" + readFileSync(pat)); // minify
				console.log(pat);
			}
        }
    }
}

loopFiles(folder);