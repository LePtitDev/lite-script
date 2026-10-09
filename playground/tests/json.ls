// Tests of JSON.parse and JSON.stringify (executed by "meson test")

// Parsing
var o = JSON.parse(" { \"name\": \"Percy\", \"age\": 17, \"tags\": [\"a\", \"b\"], \"ok\": true, \"none\": null, \"pi\": 3.5, \"big\": -2e3 } ");
assert(o.name == "Percy");
assert(o.age == 17);
assert(o.tags.length == 2 && o.tags[1] == "b");
assert(o.ok == true);
assert(o.none == null);
assert(o.pi == 3.5);
assert(o.big == -2000);

assert(JSON.parse("[]").length == 0);
assert(JSON.parse("42") == 42);
assert(JSON.parse("\"text\"") == "text");

// Escapes and unicode (surrogate pairs)
var s = JSON.parse("\"a\\n\\t\\\"\\\\\\/\\u00e9\\ud83d\\ude00\"");
assert(s.length == 8, s.length);
assert(s[1] == "\n" && s[2] == "\t" && s[3] == "\"" && s[4] == "\\" && s[5] == "/");
assert(s.charCode(6) == 233);
assert(s.charCode(7) == 128512);

// Errors
var errors = ["", "{", "[1,]", "{\"a\" 1}", "01", "1.", "\"\\x\"", "tru", "[1] 2", "\"unterminated", "\"a\nb\""];
for (var i = 0; i < errors.length; i++) {
    var failed = false;
    try {
        JSON.parse(errors[i]);
    }
    catch (e) {
        failed = e.message.startsWith("invalid JSON");
    }
    assert(failed, "JSON.parse should fail: " + errors[i]);
}

// Serialization
assert(JSON.stringify([1, 2.5, "x", true, null]) == "[1,2.5,\"x\",true,null]");
assert(JSON.stringify({ a: 1, b: [1, { c: "d" }] }) == "{\"a\":1,\"b\":[1,{\"c\":\"d\"}]}");
assert(JSON.stringify("é\n\"\\") == "\"é\\n\\\"\\\\\"");
assert(JSON.stringify({ f: function() {}, u: undefined, n: null }) == "{\"n\":null}");
assert(JSON.stringify(function() {}) == undefined);
assert(JSON.stringify({ a: [1] }, 2) == "{\n  \"a\": [\n    1\n  ]\n}");
assert(JSON.stringify({ a: 1 }, "\t") == "{\n\t\"a\": 1\n}");
assert(JSON.stringify([], 2) == "[]");

// Instances are written with their attributes
class Point {
    var x;
    var y;
    function Point(x, y) {
        this.x = x;
        this.y = y;
    }
    function norm() {
        return Math.sqrt(this.x * this.x + this.y * this.y);
    }
}
assert(JSON.stringify(new Point(3, 4)) == "{\"x\":3,\"y\":4}");

// Cycles are detected
var cycle = [1];
cycle.push(cycle);
var detected = false;
try {
    JSON.stringify(cycle);
}
catch (e) {
    detected = e.message.contains("cycle");
}
assert(detected);

// A shared value that isn't a cycle is written twice
var shared = [1];
assert(JSON.stringify([shared, shared]) == "[[1],[1]]");

// Round trip
var text = "{\"list\":[1,2,{\"deep\":[true,false,null]}],\"text\":\"héllo\",\"n\":-1.25}";
assert(JSON.stringify(JSON.parse(text)) == text);
