// Tests of File, Directory and Path (executed by "meson test")

var root = "playground-fs-test";
Directory.remove(root, true);
assert(!Directory.exists(root));
assert(Directory.create(root + "/sub/deep"));
assert(Directory.exists(root + "/sub/deep"));
assert(!Directory.create(root + "/sub/deep"));

var file = Path.join(root, "sub", "hello.txt");
assert(file == root + "/sub/hello.txt", file);
assert(!File.exists(file));
File.write(file, "héllo\nworld");
assert(File.exists(file));
assert(File.read(file) == "héllo\nworld");
assert(File.size(file) == 12);
File.append(file, "\r\n!");
var lines = File.readLines(file);
assert(lines.length == 3 && lines[0] == "héllo" && lines[1] == "world" && lines[2] == "!");

File.copy(file, root + "/copy.txt");
assert(File.read(root + "/copy.txt") == File.read(file));
File.move(root + "/copy.txt", root + "/moved.txt");
assert(!File.exists(root + "/copy.txt") && File.exists(root + "/moved.txt"));

var entries = Directory.list(root);
assert(entries.length == 2 && entries[0] == root + "/moved.txt" && entries[1] == root + "/sub", entries.join(";"));
assert(Directory.list(root, true).length == 4);

// Errors are catchable
var failed = false;
try {
    File.read(root + "/missing.txt");
}
catch (e) {
    failed = e.message.contains("can't open");
}
assert(failed);

assert(File.remove(root + "/moved.txt"));
assert(!File.remove(root + "/moved.txt"));
assert(!Directory.remove(root + "/missing"));
assert(Directory.remove(root, true));
assert(!Path.exists(root));

// Paths
assert(Path.filename("a/b/c.tar.gz") == "c.tar.gz");
assert(Path.stem("a/b/c.ls") == "c");
assert(Path.extension("a/b/c.ls") == ".ls");
assert(Path.directory("a/b/c.ls") == "a/b");
assert(Path.normalize("a/./b/../c") == "a/c");
assert(Path.absolute("x").endsWith("/x"));
assert(Path.absolute(Process.script) == Process.script);
assert(Path.filename(Process.script) == "filesystem.ls");
