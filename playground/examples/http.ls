// HTTP requests and JSON: information about a GitHub repository
// litescript-pg playground/examples/http.ls -- LePtitDev/lite-script

if (!Http.available) {
    Console.error("This playground has been built without HTTP client");
    Process.exit(1);
}

var repository = Process.args.length > 0 ? Process.args[0] : "LePtitDev/lite-script";
var response = Http.get("https://api.github.com/repos/" + repository, {
    Accept: "application/vnd.github+json"
});

if (!response.ok) {
    Console.error("Request failed with the status", response.status);
    Process.exit(1);
}

var repo = JSON.parse(response.body);
print(repo.full_name, "-", repo.description | "no description");
print("Language:", repo.language, "- stars:", repo.stargazers_count, "- forks:", repo.forks_count);
print("Created:", repo.created_at, "- updated:", repo.updated_at);

// A POST request with a JSON body (the body is converted into JSON)
var echo = Http.post("https://httpbin.org/post", { repository: repo.full_name, stars: repo.stargazers_count });
if (echo.ok)
    print("httpbin received:", JSON.stringify(JSON.parse(echo.body).json));
