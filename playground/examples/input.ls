// Read the standard input: a little calculator
// litescript-pg playground/examples/input.ls

print("Enter numbers (an empty line ends):");
var numbers = [];
while (true) {
    var line = Console.readLine("> ");
    if (line == null || line.trim() == "")
        break;
    var n = parseNumber(line);
    if (n == null)
        Console.error("\"" + line + "\" isn't a number");
    else
        numbers.push(n);
}

if (numbers.length > 0) {
    var sum = 0;
    numbers.forEach(function(n) { sum += n; });
    print("Count:", numbers.length, "- sum:", sum, "- average:", sum / numbers.length);
    numbers.sort();
    print("Min:", numbers[0], "- max:", numbers[numbers.length - 1], "- sorted:", numbers.join(", "));
}
