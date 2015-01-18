#ifndef V8MONKEY_INIT_H
#define V8MONKEY_INIT_H

/*
 * Until version 3.29.79.0, calling V8::Initialize was optional; the engine would implicitly perform initialization
 * if required. At time of writing (version 33.0) SpiderMonkey always requires explicit initialization. Thus, until
 * such time as node/spidernode catches up to the stated V8 version, we will need to carefully track SM initialization
 * status, and implicitly initialize it when required, and not rely on the user to do so.
 *
 * Another important point to note is that SpiderMonkey's initialization may fail, returning false; the V8 API states
 * that calls to V8::Initialize always return true. Thus, we also need to note whether our engine initialization
 * attempt succeeded. Further, API methods will need to ensure they return "sensible" values in the face of engine
 * initialization failure, as the user expects to be oblivious.
 *
 * (For reference: at time of writing, the only things that cause SM to fail are failures to initialize IonMonkey, and
 *  failure to correctly initialize bits related to thread-local storage.)
 *
 * Once node has caught up to recent V8s, the global for init status can go away, and our implementation of
 * V8::Initialize can simply use a static boolean or somesuch to make the call at most once. However, I believe the
 * issue around engine init failure will remain.
 *
 */

// TODO: This is apparently unused. Nice comment, though.
#define ATTEMPT_ENGINE_INIT V8::Initialize();


#endif
