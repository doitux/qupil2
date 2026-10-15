# Test signing key (public)

`qupil-test.keystore` signs the **test builds** on the nightly page
(store and key password `qupil-test`, alias `qupil-test`).

It is public on purpose: every nightly APK carries the same signature, so a
newer one installs over an older one and keeps the data on the phone. It must
never be used for a release in the Play Store; that key is kept secret and
is not part of this repository.
