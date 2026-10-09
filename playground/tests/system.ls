// Tests of Process and Time (executed by "meson test" with the arguments "first" and "second")

assert(Process.args.length == 2, Process.args.length);
assert(Process.args[0] == "first" && Process.args[1] == "second");
assert(Path.filename(Process.script) == "system.ls");
assert(["windows", "linux", "macos", "unknown"].contains(Process.platform));

assert(Process.env("LITESCRIPT_PLAYGROUND_UNDEFINED_VARIABLE") == null);
assert(typeOf(Process.env("PATH")) == "string");

var result = Process.run("echo playground");
assert(result.code == 0);
assert(result.output.trim() == "playground", result.output);
assert(Process.run("exit 3").code == 3);

var start = Time.clock();
Time.sleep(20);
assert(Time.clock() - start >= 15);
assert(Time.now() > 1700000000000);
assert(Time.iso(0) == "1970-01-01T00:00:00.000Z");
assert(Time.iso(1715934600123) == "2024-05-17T08:30:00.123Z");
assert(Time.iso().endsWith("Z"));
