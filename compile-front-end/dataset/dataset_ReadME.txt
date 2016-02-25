GA.aql文章提取信息简介：
基础的View为：
Number
Percentage
Unit
Money
Firm
Value
Change

Pattern模式为:
FirmHasValue:由Firm、Value、Money组成
FirmChange：由Firm、Change、Percentage组成

Select模式为:
PercentageOnly：提取了FirmChange中的Percentage这一列

Movies.aql文章提取信息简介:
基础View为：
Number
Month
Author
Title
Date
Appellation
Accident

Pattern模式为:
Date：由Number和Month组成
Information：由Title、Number和Month组成
Sentence：由Appellation、Accident组成

Select模式为:
TitleOnly：提取Information中的Title这一列

Music.aql文章提取信息简介：
基础的View为：
Number
Month
Year
Author
Name

Pattern模式为:
Time：由Month、Number、Year组成
Rank：由Number、Author和Name组成

Select模式为:
AuthorOnly：提取了Rank中的Author这一列