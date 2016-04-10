---
layout: post
title: Old Posts
published: true
---

----------------- 

Below is the collection of old posts from my previous blog before I migrated it over to Jekyll, I'll try sorting them out but I didn't bother saving any of the post date information so I have no idea when they were actually
written - oh well!

----------------- 


A friend of mine came to visit recently and during his visit he brought up the idea of Component Based Design and how he was using something similar for an engine he was writing in his spare time. The conversation got me very interested in the subject and since then I’ve been devouring lots of information about it. The plan now is to re-write the 2D platformer that I was writing in the vein of CBD, maybe even re-using it, or the core features, in another game afterwards.

While working on the platformer I’ve constantly had the feeling that there is much cleaner and effective way of handling many objects and entities without having to pass around a reference or pointer to a object, or other such systems, to handle rendering and the like. CBD seems to be a very effective and quite frankly cool way of handling these issues. I’ll hopefully post more once I’ve started to implement the techniques, but right now it’s refactoring time!

For those that a curious, here are some of the articles I found insightful on the topic:

http://bitsquid.blogspot.de/2011/01/managing-coupling.html
http://www.randygaul.net/2013/05/20/component-based-engine-design/
http://gameprogrammingpatterns.com/component.html



The title, in a sense, is as apocalyptic as it sounds.

Ok so, last weekend the final book in the series was released. I binge read the whole thing on the day of release, it was thrilling and quite saddening at the same time. Lets just say, the title was fitting and I’m very excited to see where the series will go. A little worried too as to see something you’ve grown up with (I’ve been playing since I was uh..12?) that you love, change this much, makes one kind of anxious.

With that out of the way, things have been quite busy lately. I’ve been studying quite a bit for various things. I recently finished Algorithms 1 from Stanford University Via coursera. Probably one of the best MOOCs I’ve done, if I’m honest. I’m currently slogging my way through Coding The Matrix: Linear Algebra, but it’s very rewarding and I’m already thinking how what I’m learning there can apply elsewhere. It’s shed a new light on the Graphics course which I took last year, the proofs etc are making more sense and I feel like re-taking it.

Anyway, the end times are also approaching for me, in terms of Uni. There’s only a few months left and we’re coming to the end to the last two courses at GU, excluding the Thesis. Talking of the Thesis, we’re starting that up now, so it’s lots of reading and writing about papers. On the subject of courses, I’ve been enrolled in a Game Design distance course at Luleå University. The course is pretty interesting, the lecturers bring up some good stuff and the literature given is very insightful. I look forward to the rest of the lectures in the coming weeks.

Game wise, not much is happening really. Attila: Total War came out back in Feb and I’ve been playing that, when I get the chance to play games. I have to admit, the changes they have made to the formula are pretty awesome. I was playing my first campaign as the Geats (Vikings, hailing from Götaland, so I had to!) and it’s really good! The difficulty has actually ramped up quite a bit. In the previous titles, Very Hard was a challenge, but it wasn’t so bad. In Attila? It had me crying like a little girl. On the note of the man himself, the way they introduce his story is really well done, so hopefully, if what the rumor mill suggests CA is cooking up next is true, we’ll see some interesting things 😉





I’ve been rather busy with school lately, but it’s actually been rather fun, despite the annoyances.

This term we had a course about Model Driven Software Development. I’d heard of it before but had never actually “done it” so to say. It was very interesting to see the differing points of view of all kinds of people when it comes to the given problem domain of a specific kind of system. In this case it happened to be a hotel management system.

Long story short, this was the first time that I’ve actually implemented a majority of what I’ve so far learnt at school into something practical. By this I mean all the software engineering theories, design, UML and anything in between. I have to say that it’s opened my eyes a little bit to how difficult this can be. On the other hand I did really enjoy the implementation phase, working closely with some good colleagues made the whole process easier, I’m really starting to like working in a team the more I do it. It was good fun to use pair programming a little bit, it’s always interesting to see what others come up with and to have to explain (at least I did, whether they wanted to know or not!) my thought process to others.

Anyway, new term, new year. Things should be a little less hectic (famous last words) this term. I found a cool story idea for the platformer which has re-invigorated me to get at least a single level with enemies, some art, parallax scrolling and some other nifty features working.

My other hobbies are going well too! The violin is getting harder, but at the same time they’re easier to pick up and grasp. Vibrato still eludes me but I think I’ll have it down in a few months. The warhammer is also going well, I’ve got the painting bug again and I’ve been working on some Sisters of Slaughter, exciting times!




As the title suggests, I’ve been having a bit of fun lately!

I delved into the magical and scary world of optimization, caches’s, memory alignment and most interestingly of all something called Data Oriented Design.

First of all, I stumbled upon cache missing from an interesting part of a website/book called gameprogrammingpatterns.com. There’s a section regarding optimization and one of the methods described was a way of using data in a way so that it’s cache friendly. Cache friendly? What is this madness? So after a day of investigating and reading I realised that it was A) something I probably should have known about already and B) Something that will probably be very important later on. I wont explain what it is here, it’s already been done by far smarter people than I. (Check wikipedia etc). This also then reminded me of memory alignment in C/C++ and got me thinking a little bit more in that area. Why use C++ if I’m not going to be/become a performance freak? 😉

Again, this led me on to finding a lot of very interesting threads, topics and articles discussing this but I then found out about DOD.

Some good pages to read more:

http://www.randygaul.net/2013/05/05/data-oriented-design-starting-up/
http://gamesfromwithin.com/

Anyway, wrapping up I started to add more things to the platformer! I decided to implement some basic AI by the use of FSM’s. I got the idea mainly from Mat Bucklands book “Programming Game AI By Example” which is very interesting. I realise that FSM’s and hierarchical FSM’s are yesteryears technology but it feels silly to just jump straight into Behaviour Trees and the like. I’ll get onto those sure enough! So in order to have the enemies be able to determine when they’re going to chase the player, I wanted to add something akin to raycasting to find if there’s a line or path existing between the player and the enemy. So I looked around and settled on using Bresenham’s line algorithm. Pretty simple to implement and it works quite well! Once I’ve got all the behavious in and working I’ll make another post.




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