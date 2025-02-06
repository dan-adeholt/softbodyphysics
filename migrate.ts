console.log("Tjena");
import fs from "fs";

// Iterate recursively over all .txt files in the current directory, using bun as interpreter

const root = "/Users/dadeholt/Source/softbodyphysics";
const folders = ["scenedefs", "levels"];

for (const folder of folders) {
  const fullFolder = root + "/" + folder;
  const files = fs.readdirSync(fullFolder);
  for (const file of files) {
    if (file.endsWith(".txt")) {
      // Open file and read lines
      let lines = fs.readFileSync(fullFolder + "/" + file, "utf-8").split("\n");

      let curStart = -1;
      let lastLine = "";
      let appendedFirst = false;

      // Iterate over lines
      for (let i = 0; i < lines.length; i++) {
        const line = lines[i];

        if (
          line.includes("disableShapeMatching") &&
          !line.includes("interiorEdges")
        ) {
          // Read start and cur from line, line starts with "start=%d end=%d"
          const start = parseInt(line.split(" ")[0].split("=")[1]);

          if (line.includes("indices=65535,65535,65535,65535")) {
            if (lastLine != "") {
              lines[i - 1] = lastLine + " interiorEdges=0,0,0,1";
              appendedFirst = false;
            }

            lines[i] = lines[i] + " interiorEdges=0,0,0,0";
            lastLine = "";
            curStart = start;
          } else {
            if (start != curStart) {
              curStart = start;

              if (lastLine != "") {
                lines[i - 1] = lastLine + " interiorEdges=0,0,0,1";
                appendedFirst = false;
              }
            } else {
              if (!appendedFirst) {
                lines[i - 1] = lastLine + " interiorEdges=0,1,0,0";
                appendedFirst = true;
              } else {
                lines[i - 1] = lastLine + " interiorEdges=0,1,0,1";
              }
            }

            lastLine = line;
          }
        } else if (!line.includes("disableShapeMatching")) {
          if (lastLine != "") {
            lines[i - 1] = lastLine + " interiorEdges=0,0,0,1";
            lastLine = "";
            appendedFirst = false;
          }
        }
      }

      // Save lines back to file
      fs.writeFileSync(fullFolder + "/" + file, lines.join("\n"));
    }
  }
}
