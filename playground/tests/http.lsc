// Tests of Http that don't need the network (executed by "meson test")

assert(typeOf(Http.available) == "boolean");

function fails(f) {
    try {
        f();
    }
    catch (e) {
        return e.message;
    }
    return null;
}

assert(fails(function() { Http.get(); }) != null);
if (Http.available) {
    assert(fails(function() { Http.get("ftp://example.com/"); }) != null);
    assert(fails(function() { Http.request({ method: "GET" }); }) != null);
}
else {
    assert(fails(function() { Http.get("http://localhost/"); }).contains("isn't available"));
}

// Set LITESCRIPT_PLAYGROUND_NETWORK to test real requests
if (Http.available && Process.env("LITESCRIPT_PLAYGROUND_NETWORK") != null) {
    var r = Http.get("https://example.com/");
    assert(r.status == 200 && r.ok);
    assert(r.headers["content-type"].startsWith("text/html"));
    assert(r.body.contains("</html>"));
}
