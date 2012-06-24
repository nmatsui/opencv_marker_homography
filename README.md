opencv_marker_homography
========================

OpenCV 2.3.1系のAPIを前提としています

ターゲット設定
--------------

1. カメラ画像からマーカー（黒い正方形）を認識する
（輪郭抽出＆折れ線近似 ⇒ 最初に見つけた頂点４つの図形を抽出）
2. マーカーの４頂点を単位格子点へ変換する射影変換行列を作成
3. マウスでクリックされた座標を射影変換行列によって直交座標系でのターゲット座標へ変換

ターゲットの描画
----------------

1. カメラ画像からマーカー（黒い正方形）を認識する
（輪郭抽出＆折れ線近似 ⇒ 最初に見つけた頂点４つの図形を抽出）
2. 単位格子点からマーカーの４頂点へ変換する射影変換行列を作成
3. ターゲット座標を射影変換行列によって現在のカメラ画像系での座標へ変換し描画

License
-------
Copyright(C) 2012 Nobuyuki Matsui (nobuyuki.matsui@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
