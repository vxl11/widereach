\ Model DEFAULT
\ LP format - for model browsing. Use MPS format to capture full model detail.
Maximize
  x1 + x2 - 60 V
Subject To
 x1: - 0.8012303532071563 w1 - 0.5024188601356272 w2 + c + x1 <= 0.999
 x2: - 0.4085014066571695 w1 - 0.8198730219744448 w2 + c + x2 <= 0.999
 y1: 0.4670604309803856 w1 + 0.6030338335289969 w2 - c - y1 <= -0.002
 y2: 0.1338289950891856 w1 + 0.4824785893751553 w2 - c - y2 <= -0.002
 y3: 0.1042381826674337 w1 + 0.442034916472025 w2 - c - y3 <= -0.002
 V: - 0.3 x1 - 0.3 x2 + 0.7 y1 + 0.7 y2 + 0.7 y3 - V <= -0.0021
Bounds
 w1 free
 w2 free
 c free
Binaries
 x1 x2 y1 y2 y3
End
