# OpenScan
Inspired by Rocketbook on Indegogo, this open source app allows the generation of printable, easy-to-scan pages, along with scanning API's to quickly and accurately scan documents directly from a camera. We believe fundamentally that document scanning should above all be *simple,* *fast,* and *acurate.* With modern cameras embedded in each and every phone, and with framerates and quality high enough to support 4k video, there is no reason that scans need to be able to process from any frame, sacrificing accuracy for universality. Instead, this software watches a stream of frames, and only accepts a scan when the borders are accurately detected, the sharpness is high enough quality, and the image is big enough to support a high-quality scan. Templates provide extra accuracy for those willing to print their pages before-hand. Finally, no scan is complete without cloud compatibility! OpenScan creates templates which can be distinguished by pre-set categories and automatically uploaded to a variety of cloud services upon scan. It also allows for optional printed selection boxes for per-page selection of cloud services without ever interacting with a menu.

This app should be in alpha by the end of the month. Current code for Desktop is available in Python and soon in C++.
