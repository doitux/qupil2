// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// iOS: the camera in a full-screen sheet; AVFoundation recognises the QR code itself
// (AVCaptureMetadataOutput), so no picture leaves the camera's pipeline.

#include "QrScanner.h"

#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

@interface QupilScanController : UIViewController <AVCaptureMetadataOutputObjectsDelegate>
@property (nonatomic, copy) NSString *prompt;
@property (nonatomic, copy) NSString *cancelTitle;
@property (nonatomic, copy) void (^done)(NSString *text);
@end

@implementation QupilScanController {
    AVCaptureSession *_session;
    AVCaptureVideoPreviewLayer *_preview;
    dispatch_queue_t _queue;
    BOOL _finished;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.backgroundColor = UIColor.blackColor;
    _queue = dispatch_queue_create("de.qupil.student.scan", DISPATCH_QUEUE_SERIAL);

    _session = [[AVCaptureSession alloc] init];
    AVCaptureDevice *device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    NSError *error = nil;
    AVCaptureDeviceInput *input = device ? [AVCaptureDeviceInput deviceInputWithDevice:device error:&error] : nil;
    if (input && [_session canAddInput:input]) {
        [_session addInput:input];
        AVCaptureMetadataOutput *output = [[AVCaptureMetadataOutput alloc] init];
        if ([_session canAddOutput:output]) {
            [_session addOutput:output];
            [output setMetadataObjectsDelegate:self queue:dispatch_get_main_queue()];
            if ([output.availableMetadataObjectTypes containsObject:AVMetadataObjectTypeQRCode])
                output.metadataObjectTypes = @[ AVMetadataObjectTypeQRCode ];
        }
    }
    _preview = [AVCaptureVideoPreviewLayer layerWithSession:_session];
    _preview.videoGravity = AVLayerVideoGravityResizeAspectFill;
    [self.view.layer addSublayer:_preview];

    // What to do, at the top
    UILabel *label = [[UILabel alloc] init];
    label.text = self.prompt;
    label.textColor = UIColor.whiteColor;
    label.font = [UIFont preferredFontForTextStyle:UIFontTextStyleHeadline];
    label.adjustsFontForContentSizeCategory = YES;
    label.numberOfLines = 0;
    label.textAlignment = NSTextAlignmentCenter;
    label.translatesAutoresizingMaskIntoConstraints = NO;
    UIView *labelBack = [[UIView alloc] init];
    labelBack.backgroundColor = [UIColor colorWithWhite:0 alpha:0.6];
    labelBack.layer.cornerRadius = 14;
    labelBack.translatesAutoresizingMaskIntoConstraints = NO;
    [labelBack addSubview:label];
    [self.view addSubview:labelBack];

    // Cancel, at the bottom
    UIButton *cancel = [UIButton buttonWithType:UIButtonTypeSystem];
    [cancel setTitle:self.cancelTitle forState:UIControlStateNormal];
    [cancel setTitleColor:UIColor.whiteColor forState:UIControlStateNormal];
    cancel.titleLabel.font = [UIFont preferredFontForTextStyle:UIFontTextStyleHeadline];
    cancel.titleLabel.adjustsFontForContentSizeCategory = YES;
    cancel.backgroundColor = [UIColor colorWithWhite:0 alpha:0.6];
    cancel.layer.cornerRadius = 14;
    cancel.translatesAutoresizingMaskIntoConstraints = NO;
    [cancel addTarget:self action:@selector(cancelTapped) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:cancel];

    UILayoutGuide *safe = self.view.safeAreaLayoutGuide;
    [NSLayoutConstraint activateConstraints:@[
        [labelBack.topAnchor constraintEqualToAnchor:safe.topAnchor constant:16],
        [labelBack.centerXAnchor constraintEqualToAnchor:safe.centerXAnchor],
        [labelBack.leadingAnchor constraintGreaterThanOrEqualToAnchor:safe.leadingAnchor constant:20],
        [labelBack.trailingAnchor constraintLessThanOrEqualToAnchor:safe.trailingAnchor constant:-20],
        [labelBack.widthAnchor constraintLessThanOrEqualToConstant:480],
        [label.topAnchor constraintEqualToAnchor:labelBack.topAnchor constant:12],
        [label.bottomAnchor constraintEqualToAnchor:labelBack.bottomAnchor constant:-12],
        [label.leadingAnchor constraintEqualToAnchor:labelBack.leadingAnchor constant:16],
        [label.trailingAnchor constraintEqualToAnchor:labelBack.trailingAnchor constant:-16],
        [cancel.bottomAnchor constraintEqualToAnchor:safe.bottomAnchor constant:-20],
        [cancel.centerXAnchor constraintEqualToAnchor:safe.centerXAnchor],
        [cancel.heightAnchor constraintGreaterThanOrEqualToConstant:50],
        [cancel.widthAnchor constraintGreaterThanOrEqualToConstant:200],
    ]];
}

- (void)viewDidLayoutSubviews
{
    [super viewDidLayoutSubviews];
    _preview.frame = self.view.bounds;
    // The picture upright also on an iPad held sideways
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    AVCaptureConnection *connection = _preview.connection;
    if (connection.isVideoOrientationSupported) {
        AVCaptureVideoOrientation orientation = AVCaptureVideoOrientationPortrait;
        switch (self.view.window.windowScene.interfaceOrientation) {
        case UIInterfaceOrientationLandscapeLeft: orientation = AVCaptureVideoOrientationLandscapeLeft; break;
        case UIInterfaceOrientationLandscapeRight: orientation = AVCaptureVideoOrientationLandscapeRight; break;
        case UIInterfaceOrientationPortraitUpsideDown: orientation = AVCaptureVideoOrientationPortraitUpsideDown; break;
        default: break;
        }
        connection.videoOrientation = orientation;
    }
#pragma clang diagnostic pop
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    AVCaptureSession *session = _session;
    dispatch_async(_queue, ^{
        if (!session.isRunning)
            [session startRunning];
    });
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    AVCaptureSession *session = _session;
    dispatch_async(_queue, ^{
        if (session.isRunning)
            [session stopRunning];
    });
    // Closed some other way (the system took the sheet away)
    [self finishWith:@""];
}

- (void)finishWith:(NSString *)text
{
    if (_finished)
        return;
    _finished = YES;
    void (^done)(NSString *) = self.done;
    self.done = nil;
    if (done)
        done(text);
}

- (void)cancelTapped
{
    [self finishWith:@""];
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)captureOutput:(AVCaptureOutput *)output
    didOutputMetadataObjects:(NSArray<__kindof AVMetadataObject *> *)objects
              fromConnection:(AVCaptureConnection *)connection
{
    (void)output;
    (void)connection;
    if (_finished)
        return;
    for (AVMetadataObject *object in objects) {
        if (![object isKindOfClass:AVMetadataMachineReadableCodeObject.class])
            continue;
        NSString *text = ((AVMetadataMachineReadableCodeObject *)object).stringValue;
        if (text.length == 0)
            continue;
        [self finishWith:text];
        [self dismissViewControllerAnimated:YES completion:nil];
        return;
    }
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    return UIDevice.currentDevice.userInterfaceIdiom == UIUserInterfaceIdiomPad ? UIInterfaceOrientationMaskAll
                                                                               : UIInterfaceOrientationMaskPortrait;
}

@end

namespace QrScanner {

namespace {

UIViewController *topViewController()
{
    UIViewController *top = nil;
    for (UIScene *scene in UIApplication.sharedApplication.connectedScenes) {
        if (![scene isKindOfClass:UIWindowScene.class])
            continue;
        for (UIWindow *window in ((UIWindowScene *)scene).windows) {
            if (window.isKeyWindow || !top)
                top = window.rootViewController;
        }
    }
    while (top.presentedViewController)
        top = top.presentedViewController;
    return top;
}

void present(const Texts &texts, std::function<void(const Result &)> done)
{
    UIViewController *top = topViewController();
    if (!top) {
        done({});
        return;
    }
    QupilScanController *controller = [[QupilScanController alloc] init];
    controller.prompt = texts.prompt.toNSString();
    controller.cancelTitle = texts.cancel.toNSString();
    controller.modalPresentationStyle = UIModalPresentationFullScreen;
    controller.done = ^(NSString *text) {
        Result result;
        result.text = QString::fromNSString(text);
        done(result);
    };
    [top presentViewController:controller animated:YES completion:nil];
}

} // namespace

bool nativeAvailable()
{
    return true;
}

void nativeScan(const Texts &texts, std::function<void(const Result &)> done)
{
    const AVAuthorizationStatus status = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    if (status == AVAuthorizationStatusAuthorized) {
        present(texts, std::move(done));
    } else if (status == AVAuthorizationStatusNotDetermined) {
        const Texts copy = texts;
        [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
            dispatch_async(dispatch_get_main_queue(), ^{
                if (granted) {
                    present(copy, done);
                } else {
                    Result result;
                    result.denied = true;
                    done(result);
                }
            });
        }];
    } else {
        Result result;
        result.denied = true;
        done(result);
    }
}

} // namespace QrScanner
