# ThingLang OOP

**ThingLang OOP** is an object-oriented superset of [ThingLang](https://github.com/Raya8877/ThingLang), created by **Intiha**. It extends the original language with events, loops, and variable management, while maintaining ThingLang's simplicity and readability.

---

## Features

- **Variables**  
  Create and manipulate global variables easily.
  
- **Events**  
  Define and trigger events with multiple callbacks.

- **Loops**  
  Repeat blocks of code using `NOTAGAIN <times>` and `TIMESEND`.

- **Bytecode Compilation**  
  Compile `.tlo` scripts to `.tloc` bytecode for faster execution.

- **Text & Bytecode Execution**  
  Run scripts directly as text or from compiled bytecode.

---

## Supported Statements

| Statement | Description |
|-----------|-------------|
| `THIS <var> IS <value>` | Define or update a variable |
| `PRINT <var/value>` | Print a variable's value or a literal |
| `TRIGGER <event>` | Trigger a registered event |
| `NOTAGAIN <times>` ... `TIMESEND` | Loop a block of statements |
| `!` | Comment (ignored) |

---

## Events

- Add callbacks in C using `add_event_callback("EventName", &callback_function);`.
- Trigger events in scripts with `TRIGGER <EventName>`.

---

## Compiler Usage

Compile a ThingLang OOP script (`file.tlo`) to bytecode:

`bash
./thinglang_oop -compile=file.tlo
# Output: file.tloc`

## Runner Usage

Run a text script (.tlo) or compiled bytecode (.tloc):

./thinglang_oop -run=file.tlo
./thinglang_oop -run=file.tloc

Example Script
<pre>
THIS Health IS 100
PRINT Health

NOTAGAIN 3
  PRINT Health
  THIS Health IS 50
TIMESEND

TRIGGER LowHealth
</pre>

## Requirements

* TCC (for tcc <pre>tloop.c -o thinglang_oop</pre> or tcc tloop -o thinglang_oop.exe)

## License
GPLv3 (because i can)
