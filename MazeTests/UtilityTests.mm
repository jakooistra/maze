//
//  MazeTests.mm
//  MazeTests
//
//  Created by John Kooistra on 2023-04-16.
//

#import <XCTest/XCTest.h>

#include "Utility.h"

@interface UtilityTests : XCTestCase

@end

@implementation UtilityTests

- (void)testGetEdgePoint2x2 {
    XCTAssertEqual(getEdgePoint(2, 2, 0).x, 0);
    XCTAssertEqual(getEdgePoint(2, 2, 0).y, 0);
    XCTAssertEqual(getEdgePoint(2, 2, 1).x, 1);
    XCTAssertEqual(getEdgePoint(2, 2, 1).y, 0);
    XCTAssertEqual(getEdgePoint(2, 2, 2).x, 1);
    XCTAssertEqual(getEdgePoint(2, 2, 2).y, 1);
    XCTAssertEqual(getEdgePoint(2, 2, 3).x, 0);
    XCTAssertEqual(getEdgePoint(2, 2, 3).y, 1);
    XCTAssertEqual(getEdgePoint(2, 2, 4).x, 0);
    XCTAssertEqual(getEdgePoint(2, 2, 4).y, 0);
}

- (void)testGetEdgePoint3x3 {
    XCTAssertEqual(getEdgePoint(3, 3, 0).x, 0);
    XCTAssertEqual(getEdgePoint(3, 3, 0).y, 0);
    XCTAssertEqual(getEdgePoint(3, 3, 1).x, 1);
    XCTAssertEqual(getEdgePoint(3, 3, 1).y, 0);
    
    XCTAssertEqual(getEdgePoint(3, 3, 2).x, 2);
    XCTAssertEqual(getEdgePoint(3, 3, 2).y, 0);
    XCTAssertEqual(getEdgePoint(3, 3, 3).x, 2);
    XCTAssertEqual(getEdgePoint(3, 3, 3).y, 1);
    
    XCTAssertEqual(getEdgePoint(3, 3, 4).x, 2);
    XCTAssertEqual(getEdgePoint(3, 3, 4).y, 2);
    XCTAssertEqual(getEdgePoint(3, 3, 5).x, 1);
    XCTAssertEqual(getEdgePoint(3, 3, 5).y, 2);
    
    XCTAssertEqual(getEdgePoint(3, 3, 6).x, 0);
    XCTAssertEqual(getEdgePoint(3, 3, 6).y, 2);
    XCTAssertEqual(getEdgePoint(3, 3, 7).x, 0);
    XCTAssertEqual(getEdgePoint(3, 3, 7).y, 1);
}

- (void)testGetEdgePoint4x3 {
    XCTAssertEqual(getEdgePoint(4, 3, 0).x, 0);
    XCTAssertEqual(getEdgePoint(4, 3, 0).y, 0);
    XCTAssertEqual(getEdgePoint(4, 3, 1).x, 1);
    XCTAssertEqual(getEdgePoint(4, 3, 1).y, 0);
    XCTAssertEqual(getEdgePoint(4, 3, 2).x, 2);
    XCTAssertEqual(getEdgePoint(4, 3, 2).y, 0);
    
    XCTAssertEqual(getEdgePoint(4, 3, 3).x, 3);
    XCTAssertEqual(getEdgePoint(4, 3, 3).y, 0);
    XCTAssertEqual(getEdgePoint(4, 3, 4).x, 3);
    XCTAssertEqual(getEdgePoint(4, 3, 4).y, 1);
    
    XCTAssertEqual(getEdgePoint(4, 3, 5).x, 3);
    XCTAssertEqual(getEdgePoint(4, 3, 5).y, 2);
    XCTAssertEqual(getEdgePoint(4, 3, 6).x, 2);
    XCTAssertEqual(getEdgePoint(4, 3, 6).y, 2);
    XCTAssertEqual(getEdgePoint(4, 3, 7).x, 1);
    XCTAssertEqual(getEdgePoint(4, 3, 7).y, 2);
    
    XCTAssertEqual(getEdgePoint(4, 3, 8).x, 0);
    XCTAssertEqual(getEdgePoint(4, 3, 8).y, 2);
    XCTAssertEqual(getEdgePoint(4, 3, 9).x, 0);
    XCTAssertEqual(getEdgePoint(4, 3, 9).y, 1);
}

@end
