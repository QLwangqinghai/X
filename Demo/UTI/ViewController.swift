//
//  ViewController.swift
//  UTI
//
//  Created by vector on 2020/6/16.
//  Copyright © 2020 clf. All rights reserved.
//

import UIKit
import MobileCoreServices
import AVFoundation

public struct Size: CustomReflectable, CustomDebugStringConvertible, Equatable {
    public var width: Int32
    public var height: Int32

    public init(width: Int32, height: Int32) {
        self.width = width
        self.height = height
    }
    
    public static let zero: Size = Size(width: 0, height: 0)

    public var customMirror: Mirror {
        let kvp = KeyValuePairs<String, Any>(dictionaryLiteral: ("width", self.width), ("height", self.height))
        return Mirror(Size.self, children: kvp, displayStyle: .struct, ancestorRepresentation: .suppressed)
    }

    public var debugDescription: String {
        return "(\(self.width), \(self.height))"
    }
    public static func == (lhs: Size, rhs: Size) -> Bool {
        return lhs.width == rhs.width && lhs.height == rhs.height
    }

}

public struct FileType {
    public var kind: UInt16
    public var format: UInt16
    public var subtype: UInt32
};

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        let per = FileType.init(kind: 0, format: 0, subtype: 0)
        // 查看结构体per所占内存
        print(MemoryLayout.stride(ofValue: per))
        // 查看结构体per实际所占内存
        print(MemoryLayout.size(ofValue: per))
        // 查看结构体per内存对齐的字节数长度
        print(MemoryLayout.alignment(ofValue: per))

        let a: AVFileType
    }


}

