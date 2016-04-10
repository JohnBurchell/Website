---
layout: post
title:  Old school post 
date:   2015-04-21 15:53:27 +0100
categories: old posts
---
The new term started last week and things have been going rather well, supervision this year sounds like it’ll take a bit of time as we’re covering 4 courses. I’m rather fond of this though as it means I’ll have an excuse to go over Erlang once again while also getting a refresher of some parts of the Software Architecture course we took previously.

I’ve been having some good fun with C++ lately, learning a few new things especially when it comes to base classes, inheritance, virtual functions and their associated ilk. I also picked up a few more idioms and acronyms! Most notable, Erase-Remove, The rules of Zero, Three and Five.

The erase remove was useful for a small lambda I wrote to act as the predicate in a remove of elements from a vector, the idiom seems to be rather straight forward especially with the in built STL functions (http://en.wikipedia.org/wiki/Erase-remove_idiom).

I’ve now run into some problems involving move semantics and smart pointers. They’re very interesting and I do like to use them where possible in my code but I’d be a liar if I said that I understood everything about them. I’ll make a post in the future regarding the issues I had and how I solved them.

Now to the inheritence. I wouldn’t say this was a huge Issue but I did have some fun figuring out when, why and what a virtual destructor was for. From my understanding, a virtual destructor (and all virtual methods) should be pure virtual at all times. The destructor being pure virtual means that an implementation is required, which seemed counter intuitive at first as normally abstract classes don’t implement anything.

This actually makes sense upon reflection and further thought as the base class’ destructor is called at the last moment. Meaning that derrived classes needed a destructor to call after their own destructors had been. One small facet which I overlooked and cause me to search in vain for some time was that I forgot to make the base destructor inline – Note to self 😉

This was also a good opportunity to start getting used to using the new Override keyword (Thanks Scott!) in my code. While common place in some other languages it’s only recently come to c++. From my understanding it’s main purpose for existence is to help the programmer catch ulgy errors and compile issues caused by incorrect inheritence. I’ve yet to have any myself (mainly because I’ve not done any complex inheritence) but I’m sure getting into the habbit of using it now will save me time in the future.

This has been the longest post yet! As usual, here comes the promise to post more often etc etc. But we all know how that turns out 😉

So i revisited the collision detection as I was a little unhappy with a few tests. I decided to re-do it and this caused a lot more issues than I thought it would. Anyway I’m back to a stage where the collision seems to be working well and all I need to finish off is getting the jumping to work smoothly.

From here on out I’m not sure what to work on next, I’d love to get some AI going into the game somehow but I’ve no idea where to start!