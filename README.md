Tomasz Kupczyk

Build & run works as described in the assignment.

For the order book I've decided to use std::multimap with key being price. Asks and bids are sorted accordingly so that guarantees price priority.
Each node corresponds to a price-level so when an order is added it's added on a list according to its arrival/time priority.
Under the hood std::multimap is a a Red-Black tree (most implementations) which will balance itself out over time as price changes.
No floating point numbers are used - just integrals.

The requirement to print the open orders after matching in arrival priority is an interesting one. I've decided that that will be maintained externally driven by
events coming from the book which of course could be done asynchronously - but I did not have time to play with that.
Another option was just to look at all open asks (then bids) and add them to a map according to timestamp (which I'm not storing, but I could) and then print.
I figured that might be a requirement to maintain some external data structure about the market driven by market changes/events hence the decision.
I'm actually not 100% sure why this requirement, why not just print balanced books? :thinking-face:

In general I write a ton of tests, currently at my work my service (ledger) is actually the only service out of around 20 that has any integration tests running on CI.
Yes, I think that code plus DB constitues an integral part of whole application behaviour. Just unit tests is definitely not enough.
I would love to add more tests where I check the state of the open order book at every stage, but figured I'll stop here, don't want to add more time.
At British Pearl we had a ton of tests for our matching logic (that was written in Scala). Order book was just sorted DB entries.

I have no idea how much time did I entirely spent. Did this at work in between meetings, during some breaks etc. Probably few good hours easily.
Some people code immediately, some people think more and need more breaks to think the problem through... different styles.
I do have a kid at home so wasn't easy to do pro-longed focused time spans after work of course.
I could easily spend hours and hours just writing tests for this.

Not a C++ expert for a long while, but did C++ long enough to hopefully not make any stupid mistakes here. Happy to work with it again for sure!

== Improvements ==

* Each type like Quantity or Price could be its own class so we get proper typing and for example don't mix up quantity or price by mistake.
* Just keeping it simple and adding one unit test file, normally would be more per component and more comprehensive ones too.
* Yes, sometimes I use struct for convenience rather than going "full-blown" class but at least deleted default constructors - just to save time where appropriate.
* Do more performance testing although was able to do around 1M trades per second on my Mac M1.
* Do more performance testing mimicking market making so new/cancel but didn't have time to implement cancel/amend etc.
[could think of some more for sure...]
