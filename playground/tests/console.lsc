// Tests of print, assert and Console (executed by "meson test")

print("print", 1, 2.5, true, null, [1, 2]);
Console.write("write ");
Console.writeLine("writeLine");

assert(true);
assert(1 == 1, "never displayed");

// A failed assertion is an error that the script can catch
var message = "";
try {
    assert(1 == 2, "one isn't two");
}
catch (e) {
    message = e.message;
}
assert(message == "assertion failed: one isn't two", message);

try {
    assert(false);
}
catch (e) {
    message = e.message;
}
assert(message == "assertion failed", message);

// The standard library is installed
assert(Math.max(1, 5, 3) == 5);
assert([3, 1, 2].sort().join("-") == "1-2-3");
