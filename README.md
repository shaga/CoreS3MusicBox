# BLE MIDI Musicbox Sequencer

## using device

* [CoreS3 SE](https://docs.m5stack.com/ja/core/M5CoreS3%20SE)
* [Base AAA](https://docs.m5stack.com/ja/base/base_aaa)
* [Module13.2 Proto](https://docs.m5stack.com/ja/module/proto13.2)
    * [Grove Mouse Encoder](https://wiki.seeedstudio.com/Grove-Mouse_Encoder/)

## Hardeware

### ハンドル
1. [クランク](hardware/3d/handle_crank.stl)と[ノブ](hardware/3d/handle_knob.stl)を3Dプリンタで出力
2. [ミニ四駆のシャフト](https://tamiyashop.jp/shop/g/g15417/)を用意して、50mmくらいに切断
3. 切ったシャフトをクランクの小さい穴に圧入
4. クランクにM3 10mmのネジを通してノブが回るように固定する
5. シャフトの片側をGrove Mouse Encoderのエンコーダに挿入

### エンコーダの接続

Module13.2 Proto基板にGrove Mouse Encoderを接続する。
    
| Core S3 (Mbus)   | Grove Mouse Encoder (Grove pin) |
|:----------------:|:-------------------------------:|
| G7 (22)          | SIG A (1)                       |
| G1 (20)          | SIG B (2)                       |
| 5V (28)          | VCC   (3)                       |
| GNC(1)           | GNC   (4)                       |

