// Count the lines of the files of a directory and save the result in JSON
// litescript-pg playground/examples/files.ls -- docs

var root = Process.args.length > 0 ? Process.args[0] : Path.directory(Process.script);
var report = { directory: Path.absolute(root), files: [], total: 0 };

var entries = Directory.list(root, true);
for (var i = 0; i < entries.length; i++) {
    if (!File.exists(entries[i]))
        continue;
    var lines = File.readLines(entries[i]).length;
    report.files.push({ path: entries[i], lines: lines, bytes: File.size(entries[i]) });
    report.total += lines;
}

report.files.sort(function(a, b) { return b.lines - a.lines; });
for (var i = 0; i < report.files.length; i++)
    print(report.files[i].lines, report.files[i].path);
print("Total:", report.total, "lines in", report.files.length, "files");

var output = Path.join(Directory.current(), "files-report.json");
File.write(output, JSON.stringify(report, 2));
print("Report written in", output);
