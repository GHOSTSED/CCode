
local userinfo_cookie_regex = "svpnuser\\s*=\\s*(?<userinfo_cookie>[^;]+);";
local cookieDict = ngx.shared.cookieDict;

-- 1. 提取请求头中的svpnuser Cookie
local cookie = ngx.header["Cookie"];
local capture, err = ngx.re.match(cookie, userinfo_cookie_regex);
local svpnuser = "";

-- 有cookie，进入权限验证阶段，此阶段可能产生的错误码：无权限（NO_PERMISSION），cookie已过期（COOKIE_ILLEGAL）
if (capture and capture["userinfo_cookie"]) then
    svpnuser = capture["userinfo_cookie"];
    local username = cookieDict:get()

-- 无cookie的情况
else

end