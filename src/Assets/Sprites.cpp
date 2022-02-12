#include "main.hpp"
#include "Assets/Sprites.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/Texture.hpp"
#include "UnityEngine/ImageConversion.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "System/Convert.hpp"
using namespace UnityEngine;
using namespace QuestUI::BeatSaberUI;

namespace MultiQuestensions {

    std::string Sprites::Oculus64 = "iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAAHdbkFIAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAAN1sAADdbAZmDAsYAAATaSURBVGhD7ZrNq1ZFHMefey21yDSxNyIKKtEyfIEgWgkhIaUFERFKK1GQJHvHZatCFymE/0ALk9zUImpViwiM3l8X1yRSVCKvZaWm5u3znfnd6zNz3u9zjtxb84EvvzNn3ufMnDMvpzcwQ2Z7Y2NjMzHb0AU0PDQ09LLuO/B8E+UxYzyA+M45DLun1JxjkXeHOM8pQH87PIm5xbt6H9AOH+nCBSgqMYF8AnkBxu/VS+G/BHU9rwrX4GmLchHzcNitDHjN8iEcR3Vv2PlE4LkKLYyF1x4fwuESCCDQalSJBU9MGTLDiae0CPMoivvIn2gPI/CYd3omEiDi1Zjfvauc/nHcn0CjDhK8DIi7VgkYG+12BvyO+iBRZrh3+9vlpcD7Jh/qYjhXDNzXY8Yb5zy6y18GXIYmPgBBFQSJXHBJ1+NrixaCx1/ev5RPLHgikaiCATOMXkHnNHoG4DN0pyVbDYFnokOK2QGaxgUELyQCOOMcWc6hA/6yFMWfg252riyv8x7cYtchFGC7K2fIIfNuDHHv90kEnDVvR/zdy853er1riTTXrpuy1mw/Z8w6wm8CkNlpzGzv6oRlPIKv7Do7O8TzCszz3tUqo2hef+aV0Bqz0TNoBDXlLNqHVlhyiUQikUjkkvkcx/AxuQFzO7oHaRns9/Py0ddVy8PDaD86yNcvmIDUhowXI00oB0HbT6+i3E2hQoiwRbFbRAVR61VDwPUuSvscRtrkDYhnxdrQ1TPsin30icfs2hEXYCcmux3oUcc6iao6rqbld6DLnSsLZShIggIcQTHHUONJKnE2uNhZHrEgWfDUPC5mlXk3hrg/+yQCdpi3Ix4e2sqJ+cHsZPjDbD9XmXXEBdCGXMxLZhtBTTXsbvOugBGzjrgTfo5Z7l0B7yCdkdQdIVoXvofyWvRGOuHEjmNcgLsx+Ztg7fA3mQcdOngEeH6D6aoAGp55rRtCK8xFx1HbNFvuEWG/jzcw2r5cbck2g4gr0Fso7/1QxQH0LOpypZ1IJBKJRCKRSCSmMVXbLIWw0NCpmn4TWYnuQ8uQVsN5K+E20cpch6g6dfsYfYi+ZT2rY8buoeIPoU/RKTRVUFlUpjVWzHYhYS3UX0BdLNbbZhS9iOZZ8UupHAIktBizFy1Bkx4ylxhtwehvrccZGt+7OwWUVojKL8Vor3+WuzH90PnEvTTCF96ZpbABqLwOR95F1ZtZHr2c3ka7kbb+9JtJm+jlqjJtRg+jov3vmC/RgzTCEe+sCQ2gXxVOoirOoF2o3hlIC5DXHLQTnUZVqA7PWdQMZadHOmUItrIL+BHtoIV1enFJIC9tu2uf/6C7UY7qoGO+XMoaQJnU2Q3Xk6//b1h7KM86vU51yDt2KIduswblHRnloWOkTajzXVjlgTZannVQHfJ+4HKUvQRlXkNbddEAtfgvSK1edqrcBPVUdeXrUNOZ5i60lWHjXRFVn0Edm76BnnA3ph+av6yj8v945ySgEfQ75zqUd+A4VdHvpzpwzxyMTxoS06dnMxr0h9ouUdmeQvp9tRalQyAPEtd41CdyPXoAaaLU9QqwCL1vNNF5H2mojtDdG713GjdADA2iGdl8pMXHNehWtABdidQwg+aht7Eqegr9in5CJ9BvaJQKtz3jTCQSiUQi8b+g1/sXRmYDomhz93gAAAAASUVORK5CYII=";
    std::string Sprites::Steam64 = "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";

    Sprite* CreateSpriteFromBase64(std::string& base64) {
        ArrayW<uint8_t> bytes = System::Convert::FromBase64String(il2cpp_utils::newcsstr(base64));
        Texture2D* texture = Texture2D::New_ctor(2, 2);
        if (ImageConversion::LoadImage(texture, bytes, false)) {
            return Sprite::Create(texture, UnityEngine::Rect(0.0f, 0.0f, (float)texture->get_width(), (float)texture->get_height()), UnityEngine::Vector2(0, 0), 10.0f, 0u, SpriteMeshType::Tight, UnityEngine::Vector4(0.0f, 0.0f, 0.0f, 0.0f), false);
        }
        return nullptr;
    }

    Sprite* Sprites::IconOculus64() {
        return CreateSpriteFromBase64(Oculus64);
    }

    Sprite* Sprites::IconSteam64() {
        return CreateSpriteFromBase64(Steam64);
    }
}