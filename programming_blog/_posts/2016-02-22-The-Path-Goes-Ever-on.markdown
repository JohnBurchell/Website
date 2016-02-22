---
layout: post
title: The Path Goes Ever on and on..
published: true
---
Recently I've been working on some rather fun programming exercises and the most interesting of them by far was a particular programming puzzle. You're basically given a grid search space to work with (that's actually just a linear array) and need to find a given target from a start position. Simple enough right? I wanted to see what of the algorithms I knew were the fastest so I started with BFS, Dijkstra and then tried A*. I decided to then have a little look at all the kinds of optimisations that are available to A*, as there are quite a few to say the least.

Some of the ones that caught my eye were [Jump Point Search]{https://harablog.wordpress.com/2011/09/07/jump-point-search/} and [Hierarchical Pathfinding (HPA*)]{http://aigamedev.com/open/review/near-optimal-hierarchical-pathfinding/}. I went with the former and it seems to work very well. On the test maps I had it was blazingly fast (in comparison to the A* implementation) and when I tested it on a map of 250,000 nodes it was similarly very quick. It did not however pass the submission and actually timed out, which is quite perplexing! So I'm left with the choice of more optimisations or perhaps giving HPA* a shot. However, the authors of the JPS paper state that: 
	
	"A* + JPS was able to consistently outperform the well known HPA* algorithm. This is remarkable as A* + JPS is always performing optimal search while HPA* is only performing approximate search."

So it's possible that I just have a lousy implementation or that the test is doing something that I don't expect it to do! Both are probable, but we'll see how things go.

[GameAIPro]{http://www.gameaipro.com/} also has been useful while writing the JPS implementation, there are many tips for optimisation inside that I could try applying to the sollution, some of which I've already taken on board. For example, it was suggested to use a more appropriate heuristic, so I tried out the octile heurustic suggested in the book. Furthermore, there was the suggestion that (as accocrding to the book) all game devs should adhere to which was to pre-allocate all the memory required before starting the search. I then applied this logic to the search and from my own tests (in debug at least) I shaved two seconds off the original search time. Alas, this was still insufficient for the auto grader so I'll have to do some more thinking and try again soon.


